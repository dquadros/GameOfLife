
#define SSD1306_SDA 16
#define SSD1306_SCL 17

// SSD1306 I2C Address
#define DISP_ADDR  0x3C

// SSD1306 Commands
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3

// Halt execution
void halt() {
  while(1) {
    delay(100);
  }
}

// SSD1306 initialization
const uint8_t cmdInit[] =
{
    SSD1306_DISPLAYOFF,
    SSD1306_SETDISPLAYCLOCKDIV, 0x80,
    SSD1306_SETMULTIPLEX, 0x3F,
    SSD1306_SETDISPLAYOFFSET, 0x00,
    SSD1306_SETSTARTLINE | 0x0,
    SSD1306_CHARGEPUMP, 0x14,
    SSD1306_MEMORYMODE, 0x00,
    SSD1306_SEGREMAP | 0x1,
    SSD1306_COMSCANDEC,
    SSD1306_SETCOMPINS, 0x12,
    SSD1306_SETCONTRAST, 0xCF,
    SSD1306_SETPRECHARGE, 0xF1,
    SSD1306_SETVCOMDETECT, 0x40,
    SSD1306_DISPLAYALLON_RESUME,
    SSD1306_NORMALDISPLAY,
    SSD1306_DISPLAYON
};

// Init display
void Display_init() {
  // Init Wire Library
  if (!Wire.setSCL(SSD1306_SCL) or !Wire.setSDA(SSD1306_SDA)) {
    Serial.println("Error setting SCL/SDA");
    halt();
  }
  Wire.setClock(400000);
  Wire.begin();

  // Check display connection
  Wire.beginTransmission(DISP_ADDR);
  uint8_t error = Wire.endTransmission();
  if (error) {
    Serial.print("Error accessing display: ");
    Serial.println (error);
    halt();
  }

  // Init controller
  Display_sendcmd (cmdInit, sizeof(cmdInit));

  // Clear screen
  memset(screen, 0, SCREEN_SIZE);
  Display_update();
}

// Send current screen to display
void Display_update() {
  Display_sendcmd (SSD1306_COLUMNADDR);
  Display_sendcmd (0);
  Display_sendcmd (SCREEN_WIDTH-1);
  Display_sendcmd (SSD1306_PAGEADDR);
  Display_sendcmd (0);
  Display_sendcmd (7);
  for (uint16_t i = 0; i < SCREEN_SIZE; i+=64)
    {
        Wire.beginTransmission(DISP_ADDR);
        Wire.write(0x40);  // Co=0, DC = 1
        Wire.write(screen+i, 64);
        int error = Wire.endTransmission();
        if (error) {
          Serial.print("Error: ");
          Serial.println(error);
        }
    }
}

// Envia sequência de comandos ao display
void Display_sendcmd (const uint8_t *cmd, int nCmds)
{
  Wire.beginTransmission(DISP_ADDR);
  Wire.write (0);  // Co= 0. DC = 0
  for (int i = 0; i < nCmds; i++)
  {
    Wire.write(cmd[i]);
  }
  int error = Wire.endTransmission();
  if (error) {
    Serial.print("Error: ");
    Serial.println(error);
  }
}

// Envia um comando ao display
void Display_sendcmd (uint8_t cmd)
{
  Wire.beginTransmission(DISP_ADDR);
  Wire.write (0);  // Co= 0. DC = 0
  Wire.write(cmd);
  int error = Wire.endTransmission();
  if (error) {
    Serial.print(" Error: ");
    Serial.println(error);
  }
}
