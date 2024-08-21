import socket
import struct
import time
import numpy as np

#  判断反馈情况
def select_vibration_modes(grouped_errors):
    """根据规则分类每组数字"""
    result = []
    for errors in grouped_errors:

        # 取误差绝对值
        abs_values = np.abs(errors)

        # 若所有关节角误差值在10度以内，停止振动
        all_less_equal_10 = np.all(abs_values <= 10)
        if all_less_equal_10:
            result.append(0)
            continue


        # 考虑关节移动关联性
        modified_values = np.array([abs_values[0] + 10, abs_values[1] + 5, abs_values[2]])

        # 确定优先校正的关节
        max_index = np.argmax(modified_values)

        # 确定校正方向：flexion 或extension
        output = max_index * 2 + 1 if errors[max_index] > 0 else max_index * 2 + 2

        result.append(output)

    return tuple(result)


#  进行反馈生成
def generate_vibration(finger_vibration_modes):

    # 设置5个VibroBot的接收端口(同局域网下)
    send_addr_1 = ('192.168.3.35', 1234)
    send_addr_2 = ('192.168.3.37', 1234)
    send_addr_3 = ('192.168.3.38', 1234)
    send_addr_4 = ('192.168.3.39', 1234)
    send_addr_5 = ('192.168.3.40', 1234)

    send_addrs = [send_addr_1, send_addr_2, send_addr_3, send_addr_4, send_addr_5]

    i = -1
    for send_addr in send_addrs:
        i = i + 1

        vibration_mode = finger_vibration_modes[i]
        message = f"370 {vibration_mode}".encode()
        #send_sock.sendto(message, send_addr)
        print(f"Message sent to {send_addr}: {message}")

        # 发送后延迟2秒
        time.sleep(2)

    return 0

# UDP服务器设置：指节误差数据
UDP_IP = "127.0.0.1"
UDP_PORT = 12345

# 创建接收socket：非阻塞模式
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))
sock.setblocking(False)

print(f"Listening gesture errors for UDP packets at {UDP_IP}:{UDP_PORT}...")

# 创建发送socket
# 创建另一个socket用于发送振动模式设置到VibroBot
send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:

        # 清空缓冲区
        while True:
            try:
                sock.recvfrom(65535)  # 尝试接收数据并丢弃，65535是最大可能的UDP数据包大小
            except socket.error as e:
                # 当没有更多数据可读时，非阻塞套接字会抛出异常
                break
        try:
            # 接收最新数据
            sock.setblocking(True)
            data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes

            # 检查接收到的数据长度是否是4的倍数（float大小）
            if len(data) % 4 == 0:

                # 转换为浮点数
                floats = struct.unpack(f'{len(data) // 4}f', data)

                # 将15个数分为5组，每组3个，对应每个手指
                grouped_errors = [floats[i:i + 3] for i in range(0, 15, 3)]
                print(f"Received from {addr}: {grouped_errors}")

                # 根据每个手指的误差数据进行设置振动模式反馈
                finger_vibration_modes = select_vibration_modes(grouped_errors)
                print(f"Finger vibration_modes: {finger_vibration_modes}")
                generate_vibration(finger_vibration_modes)

                # 重设为非阻塞模式准备下一轮清空
                sock.setblocking(False)
            else:
                print(f"Received data from {addr} but it's not a valid float array")

        except BlockingIOError:
            # 没有数据接收时，会抛出 BlockingIOError
            pass


except KeyboardInterrupt:
    print("\nUDP Server shutting down")
finally:
    sock.close()
