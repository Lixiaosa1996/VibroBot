#include <WiFi.h>
#include <WiFiUdp.h>
#include <stdlib.h>

// WiFi网络信息
const char* ssid = "HUAWEI-R1CL5C";
const char* password = "12345678";

WiFiUDP udp;
unsigned int localUdpPort = 1234;  // 本地UDP端口


// LEDC pwm输出信息
const int ledPin = 25;         // 使用ESP32的25号引脚
const int ledChannel = 0;     // 选择一个LED通道
const int resolution = 8;     // 分辨率设为8位 (实际占空比范围为0-1000)
const int pwmFrequency = 370; // PWM频率设置为370Hz， pwm波频率10kHz


// 设置振动波形周期与间隔周期(ms)
int period_num = 20;
unsigned long time_vibra = 200; //振动波频100Hz
unsigned long time_gap = 50;
unsigned long time_vibra_all = (time_vibra + time_gap) * period_num;

void setup() {
  Serial.begin(115200);

  // 连接到WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  // 启动UDP
  udp.begin(localUdpPort);
  Serial.print("Now listening at IP ");
  Serial.print(WiFi.localIP());
  Serial.print(", UDP port ");
  Serial.println(localUdpPort);

  // 初始化LED PWM功能
  ledcSetup(ledChannel, pwmFrequency, resolution);
  ledcAttachPin(ledPin, ledChannel);
  ledcWrite(ledChannel, 0);

}

void loop() {
  int packetSize = udp.parsePacket();

  if (packetSize) {
    // 读取接收到的数据
    uint8_t incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0) {
      incomingPacket[len] = 0; // 确保字符串以null结尾

       // 向发送者回送数据
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write((const uint8_t *)incomingPacket, len);
      udp.endPacket();
      
      char input_str[sizeof(incomingPacket)];

      // 将uint8_t数组转换为字符数组
      for (size_t i = 0; i < sizeof(incomingPacket) - 1; i++) {
          input_str[i] = (char)incomingPacket[i];
      }

      char *token;
      int values[2];
      int i = 0;

      // 使用strtok函数拆分字符串
      token = strtok(input_str, " ");
      while (token != NULL && i < 2) {
        // 使用atoi函数将子字符串转换为整数
        values[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
      }

      // 更新pwm频率和占空比，持续10s      
      int freq = values[0];
      int waveType = values[1];
      if (i == 2) {
        Serial.printf("Received packet: freq:%d  waveType:%d\n", freq, waveType);
        ledcWriteTone(0, freq);
        // ledcWrite(ledChannel, dutyRatio); //占空比设置
        //波形设置
        if(waveType == 1)    // MCP-Flexion
        {
          period_num = 2;
          time_vibra = 200;
          time_gap = 50;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else if (waveType == 2)    // MCP-Extension
        {
          period_num = 2;
          time_vibra = 400; 
          time_gap = 100;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else if (waveType == 3)    // PIP-Flexion
        {
          period_num = 3;
          time_vibra = 200; 
          time_gap = 50;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else if (waveType == 4)    // PIP-Extension
        {
          period_num = 3;
          time_vibra = 400; 
          time_gap = 100;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else if (waveType == 5)    // DIP-Flexion
        {
          period_num = 4;
          time_vibra = 200; 
          time_gap = 50;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else if (waveType == 6)   // DIP-Extension
        {
          period_num = 4;
          time_vibra = 400; 
          time_gap = 100;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }
        else  // No vibration
        {
          period_num = 0;
          time_vibra = 0; 
          time_gap = 0;
          time_vibra_all = (time_vibra + time_gap) * period_num;
        }

        // 记录开始时间
        unsigned long startTime = millis(); //单位ms
        unsigned long time_diff;

        do{
          // 获取当前时间差
          unsigned long currentTime = millis();
          time_diff = currentTime - startTime; 

          // 根据振动周期映射到PWM占空比范围
          int dutyCycle = 550;
          Serial.println(dutyCycle);
          
          // 应用调制的PWM占空比
          ledcWrite(ledChannel, dutyCycle);
          
          // 因为我们正在模拟一个连续变化的波形，所以不需要太长的延迟
          delayMicroseconds(100); // 短暂延迟确保能够看到平滑的变化??

          // 判断是否刚好完成一个振动信号周期
          currentTime = millis();
          time_diff = currentTime - startTime;
          if(abs(time_vibra - time_diff % (time_vibra+time_gap)) < 5)
          { 
            // 在gap部分停止振动
            ledcWrite(0, 0);
            delay(20);
            currentTime = millis();
            time_diff = currentTime - startTime;
            while(time_diff % (time_vibra+time_gap)>time_vibra) 
            {
              delay(1);
              currentTime = millis();
              time_diff = currentTime - startTime;
            }                   
          } 
        }while(time_diff < time_vibra_all && waveType >= 1 && waveType <= 6); 
      }

      ledcWrite(0, 0);

    }
  }
}
