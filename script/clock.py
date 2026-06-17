"""
YourLovelyClock BLE Terminal
专用于连接 STM32 BLE UART 模块

注意：
- 此脚本默认蓝牙模块名称为 "YourLovelyClock"，请根据实际情况修改 TARGET_NAME 变量。
- 此脚本是借助 AI 编写的。
"""

import asyncio
from bleak import BleakScanner, BleakClient

TARGET_NAME = "YourLovelyClock"

# 你这个模块通常是 HM-10 / BT05 风格 UUID
UART_SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb"

class ClockBleTerminal:
    def __init__(self):
        self.client = None
        self.char = None

    async def find_device(self):
        """扫描并寻找目标设备"""
        print("🔍 正在扫描 YourLovelyClock ...")

        devices = await BleakScanner.discover(timeout=5.0, return_adv=True)

        matched = []

        for device, adv in devices.values():
            if device.name == TARGET_NAME:
                matched.append((device, adv.rssi))

        if not matched:
            print("❌ 未找到 YourLovelyClock")
            return None

        # RSSI 最大（最接近 0）的设备优先
        matched.sort(key=lambda x: x[1], reverse=True)

        device, rssi = matched[0]

        print(f"✅ 找到设备: {device.address} RSSI={rssi} dBm")

        return device

    async def find_uart_char(self):
        """
        自动寻找可通信特征值：
        - notify
        - write 或 write-without-response
        """

        for service in self.client.services:

            # 只看 Vendor UART 服务
            if service.uuid.lower() != UART_SERVICE_UUID:
                continue

            for char in service.characteristics:

                props = set(char.properties)

                can_notify = "notify" in props
                can_write = (
                    "write" in props or
                    "write-without-response" in props
                )

                if can_notify and can_write:
                    print("✅ 已自动找到 UART 特征值")
                    print(f"UUID: {char.uuid}")
                    print(f"属性: {', '.join(char.properties)}")
                    return char

        return None

    def on_notify(self, characteristic, data):
        """接收数据"""

        try:
            text = data.decode("utf-8")
            print(text, end="", flush=True)

        except UnicodeDecodeError:
            print(f"\n📥 HEX: {data.hex()}")

    async def terminal_loop(self):
        """终端交互"""

        print("\n==============================")
        print("✅ 已进入 BLE 调试终端")
        print("输入 quit 退出")
        print("==============================\n")

        # 发送初始消息 "hello"
        try:
            hello_data = "hello".encode("utf-8")
            await self.client.write_gatt_char(
                self.char.uuid,
                hello_data
            )
            print("Clock> hello")
        except Exception as e:
            print(f"\n❌ 发送 hello 消息失败: {e}")

        while self.client.is_connected:

            try:
                line = await asyncio.get_event_loop().run_in_executor(
                    None,
                    input
                )

                if line.lower() == "quit":
                    break

                # 支持 HEX 发送
                if line.startswith("0x"):
                    data = bytes.fromhex(line[2:])
                else:
                    data = line.encode("utf-8")

                await self.client.write_gatt_char(
                    self.char.uuid,
                    data
                )

            except Exception as e:
                print(f"\n❌ 发送失败: {e}")
                break

    async def run(self):

        # 1. 找设备
        device = await self.find_device()

        if not device:
            return

        # 2. 连接
        print("🔗 正在连接...")

        async with BleakClient(device.address) as client:

            self.client = client

            print("✅ BLE 已连接")

            # 3. 找 UART characteristic
            self.char = await self.find_uart_char()

            if not self.char:
                print("❌ 未找到 UART 特征值")
                return

            # 4. 开启 notify
            if "notify" in self.char.properties:

                await client.start_notify(
                    self.char.uuid,
                    self.on_notify
                )

                print("📡 已开启通知监听")

            # 5. 进入终端
            await self.terminal_loop()

            # 6. 关闭 notify
            try:
                await client.stop_notify(self.char.uuid)
            except:
                pass

        print("\n👋 已断开连接")


def main():

    print("🚀 YourLovelyClock BLE Terminal")

    try:
        asyncio.run(ClockBleTerminal().run())

    except KeyboardInterrupt:
        print("\n程序退出")

    except Exception as e:
        print(f"\n❌ 错误: {e}")


if __name__ == "__main__":
    main()