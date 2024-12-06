#include <SPI.h>
#include <mcp2515.h>

struct can_frame read_can_msg;
struct can_frame write_can_msg;
MCP2515 mcp2515(10);


void setup() {
  Serial.begin(9600);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ);
  mcp2515.setNormalMode();
}

void loop() {



  if (mcp2515.readMessage(&read_can_msg) == MCP2515::ERROR_OK) {
    memset(&write_can_msg, 0, sizeof(write_can_msg));
    int potent_rpm = analogRead(A0);
    unsigned char p_id = read_can_msg.data[2];
    int a = ((int)read_can_msg.data[2]);
    int b = ((int)read_can_msg.data[4]);


    /*Serial.print("요청된 PID: ");
    Serial.println(a);  //22인경우 확장 PID
    Serial.print("확장 PID 구분자: ");
    Serial.println(b);  //여기로 확장 PID 구분*/





    switch (p_id) {
      case 0:                         //PID 0번이면 가용 가능한 목차 전송
        write_can_msg.can_id = 2024;  //ECU ID넘버
        write_can_msg.can_dlc = 8;    //바이트 길이수
        write_can_msg.data[0] = 6;    //길이
        write_can_msg.data[1] = 65;   // 서비스 번호
        write_can_msg.data[2] = 0;    //pid
        write_can_msg.data[3] = 255;  //데이터(속도 RPM등)
        write_can_msg.data[4] = 255;
        write_can_msg.data[5] = 255;
        write_can_msg.data[6] = 255;
        write_can_msg.data[7] = 255;

        mcp2515.sendMessage(&write_can_msg);
        //Serial.println("사용가능한 리스트전송완료");
        
        break;

      /*case 12:
        write_can_msg.can_id = 2024;
        write_can_msg.can_dlc = 8;
        write_can_msg.data[0] = 4;
        write_can_msg.data[1] = 65;
        write_can_msg.data[2] = 12;
        write_can_msg.data[3] = (long)(potent_rpm * 4) / 256;
        write_can_msg.data[4] = (long)(potent_rpm * 4) % 256;
        write_can_msg.data[5] = 170;
        write_can_msg.data[6] = 170;
        write_can_msg.data[7] = 170;

        mcp2515.sendMessage(&write_can_msg);
        Serial.println("rpm 값 전송");
        break;*/

      case ((22, HEX) == ((int)read_can_msg.data[2], HEX)):
      Serial.println("0");
        if ((1, HEX) == ((int)read_can_msg.data[4], HEX)) {
          Serial.println("x");
          write_can_msg.can_id = 2024;  //ECU ID넘버
          write_can_msg.can_dlc = 8;    //바이트 길이수
          write_can_msg.data[0] = 16;   //0x10 (256*1)
          //첫 번째 바이트: 0x1X 형태로, 여기서 X는 데이터의 총 길이의 첫 두 자리(상위 4비트)입니다. 예를 들어, 총 데이터 길이가 16바이트라면 첫 번째 바이트는 0x10이 됩니다.
          write_can_msg.data[1] = 0;  //두번째 바이트는 0이므로 총 데이터 길이는 256 바이트 -> 데이터 프레임을 32번 보내야됨
          // 첫 프레임(First Frame)에서 두 번째 바이트는 전체 데이터의 총 길이의 하위 8비트를 나타냅니다.
          write_can_msg.data[2] = 0;    // 더미 a
          write_can_msg.data[3] = 240;  // 더미 b
          write_can_msg.data[4] = 230;  // 더미 c
          write_can_msg.data[5] = 222;  //a d
          write_can_msg.data[6] = 210;  //b e
          write_can_msg.data[7] = 105;  //c f

          mcp2515.sendMessage(&write_can_msg);


         
          write_can_msg.data[0] = 33;   //21 첫번째 프레임 의미 14바이트 전송
          //첫 번째 바이트: 0x2N 형태로, 여기서 N은 순차적인 프레임 번호(1~F)입니다. 첫 후속 프레임은 0x21, 그 다음은 0x22, 이런 식으로 증가
          //이후 바이트들은 그냥 데이터
          write_can_msg.data[1] = 10;   //d g
          write_can_msg.data[2] = 20;   //e -> e/2 해서 % 로 나누면 현재 배터리 잔량 % h
          write_can_msg.data[3] = 30;   //f i
          write_can_msg.data[4] = 2;    //g j
          write_can_msg.data[5] = 2;    //h k
          write_can_msg.data[6] = 1;    // i l
          write_can_msg.data[7] = 126;  //j -> 충전중인지 아닌지 판별 1000 0000 <- {J:7} 128이면 충전중 m

          mcp2515.sendMessage(&write_can_msg);


          
          write_can_msg.data[0] = 34;   //두번째 프레임 22바이트 전송
          write_can_msg.data[1] = 2;    //k n
          write_can_msg.data[2] = 2;    //l o
          write_can_msg.data[3] = 255;  //m p
          write_can_msg.data[4] = 40;   //n q
          write_can_msg.data[5] = 50;   //o
          write_can_msg.data[6] = 200;  //p
          write_can_msg.data[7] = 30;   //q Signed(Q) bat moudle 01

          mcp2515.sendMessage(&write_can_msg);


          
          write_can_msg.data[0] = 35;   //세번째 프레임 30바이트 전송
          write_can_msg.data[1] = 40;   //r Signed(r) bat moudle 02
          write_can_msg.data[2] = 50;   //s Signed(s) bat moudle 03
          write_can_msg.data[3] = 60;   //t Signed(t) bat moudle 04
          write_can_msg.data[4] = 255;  //u
          write_can_msg.data[5] = 255;  //v
          write_can_msg.data[6] = 255;  //w
          write_can_msg.data[7] = 170;  //x1

          mcp2515.sendMessage(&write_can_msg);


          
          write_can_msg.data[0] = 36;   //네번째 프레임 38바이트 전송
          write_can_msg.data[1] = 20;   //y2
          write_can_msg.data[2] = 22;    //z 3
          write_can_msg.data[3] = 222;  //aa 4(T)
          write_can_msg.data[4] = 255;  //ab
          write_can_msg.data[5] = 255;  //ac
          write_can_msg.data[6] = 255;  //ad
          write_can_msg.data[7] = 170;  //ae

          mcp2515.sendMessage(&write_can_msg);

          
          write_can_msg.data[0] = 37;   //다섯번째 프레임 38바이트 전송
          write_can_msg.data[1] = 20;   //af
          write_can_msg.data[2] = 11;    //ag
          write_can_msg.data[3] = 222;  //ah
          write_can_msg.data[4] = 255;  //ai
          write_can_msg.data[5] = 255;  //aj
          write_can_msg.data[6] = 0;    //ak
          write_can_msg.data[7] = 0;    //al

          mcp2515.sendMessage(&write_can_msg);


          Serial.println("01 번 PID 전송 완료");
        }
        break;

      default:
        break;
    }


  }
}