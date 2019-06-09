int showOnLCD(char text[msgLength]) {
    connectLCD();
    setRGB(0, 102, 204);
    setText(text);
}