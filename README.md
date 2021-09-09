# ESP32-NFC-door
This program opens a door (activating a relay) when the correct NFC card is passed through the reader

The way of changing which NFC cards are authorized is by connecting to the Wifi Access-point the ESP32 creates and enter the next IP: 192.168.4.1
This shows the next interface:
![Interface](https://i.imgur.com/vcxWyxI.png)

Here the user can see the list of authorized NFC cards with the name saved and the UID
The button "Add with name" will add the last non authorized card that was passed through the reader

This program SHOULD NEVER be used to protect anything remotly valuable. It's meant to be used only in educational purpose. Here are some reasons why you shouldn't use it:
- The reader reads only the UID, which is NOT UNIQUE, meaning a card that was never granted access, can open the door (if it has the same UID as a authorized card). This was done so bus and university cards could be used.
- The Access point to grant acces to editing which cards are authorized, has only one password and if not set up correctly can be cracked. (And it's based of a basic connecting, doesn't have encryption per se).
- If the circuit is known and the relay or esp32 is close to the exterior, activating the relay with some magnet or by inducing 3.3v on the S terminal would be enough to open the door bypassing the esp32 altogheter.





