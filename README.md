รายการอุปกร
1. ESP8266 wemos D1 mini
2. MQ-2 sensor
3. ATH20
4. buzzer
5. Relay 2CH (5V)
6. Resistor 50K + 6K, 20K for voltage divider
7. 3.3v LED
8. wires
9. 5V power source
10. 3.3V power source

การควบคุม Relay

ตัวบอร์ด subscribe topic mac/FF:FF:FF:FF:FF:FF/# (แทนที่ FF:FF:FF:FF:FF:FF ด้วย mac address ของตัวบอร์ด)

สั่งเปิด Relay Channel 1 ใช้ topic mac/FF:FF:FF:FF:FF:FF/cmd และส่งค่า "On" ตัวบอร์ดจะส่ง LOW ให้ขา GPIO 0

สั่งเปิด Relay Channel 1 ใช้ topic mac/FF:FF:FF:FF:FF:FF/cmd และส่งค่า "Off" ตัวบอร์ดจะส่ง HIGH ให้ขา GPIO 0

สั่งเปิด Relay Channel 2 ใช้ topic mac/FF:FF:FF:FF:FF:FF/cmd และส่งค่า "OnB" ตัวบอร์ดจะส่ง LOW ให้ขา GPIO 2

สั่งเปิด Relay Channel 2 ใช้ topic mac/FF:FF:FF:FF:FF:FF/cmd และส่งค่า "OffB" ตัวบอร์ดจะส่ง HIGH ให้ขา GPIO 2


ตัวบอร์ดส่งค่าความแรงของ wifi (RSSI) ด้วยการ publish topic mac/FF:FF:FF:FF:FF:FF/signal (แทนที่ FF:FF:FF:FF:FF:FF ด้วย mac address ของตัวบอร์ด)

ตัวบอร์ดส่งค่าอุณหภูมิและความชื้น รวมถึง state ของ pin 12(pin ที่อ่าน digital output ของ MQ-2) ด้วยการ publish topic mac/FF:FF:FF:FF:FF:FF/weather (แทนที่ FF:FF:FF:FF:FF:FF ด้วย mac address ของตัวบอร์ด)

-โดย payload จะมีตัวอย่างดังนี้ "macS:4C:75:25:36:C0:29:t=32.388996:h=81.082977:mq=1" 
-ประกอบด้วย
  macS คือเลข macaddress ของบอร์ด (ให้ subscribe topic mac/# เพื่อดูว่าบอร์ดที่ติดต่อกับ MQTT broker นั้นมีเลขแมคแอดเดรสอะไร)
  t คือค่าอุณหภูมิ
  h คือค่าความชื้นสัมพัท
  mq คือค่าที่อ่านได้จาก pin ที่ต่อกับเซนเซอร์ MQ-2
