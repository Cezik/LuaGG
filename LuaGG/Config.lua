-- Numer, na kt�ry program b�dzie dzia�a�
LoginNumber = 123456

-- Has�o do powy�szego numeru
LoginPassword = "Your_Password_Here"

--[[
Status, na kt�rym program b�dzie dzia�a�
1 = PoGGadaj ze mn�
2 = Dost�pny
3 = Zaraz Wracam
4 = Niewidoczny
5 = Nie przeszkadza�
6 = PoGGadaj ze mn� + Opis
7 = Dost�pny + Opis
8 = Zaraz Wracam + Opis
9 = Nie przeszkadza� + Opis
10 = Niewidoczny + Opis
]]--
LoginStatus = 1

-- Opis po zalogowaniu
LoginDesc = "Pozdrawiam!"

-- Czy program powinien pokazywa� okienka z podpowiedziami? (Tool Tips) [true/false]
ShowToolTips = false

-- Czy program powinien automatycznie ��czy� si�, je�eli zostanie roz��czony?
autoReConnect = true

-- Ilo�� pr�b ponownych po��cze�
autoReConnectTries = 5

-- Akceptuj adresy WWW od nieznajomych? (Wymaga r�cznej zmiany statusu) [true/false]
acceptLinks = false

-- Zapisywa� logi z konsoli do pliku tekstowego? [true/false]
saveLogs = true

-- Czy program powinien u�ywa� systemu "multi logowania" ? [true/false]
useMultiLogOn = true

-- Czas (w milisekundach) po, kt�rym ma by� wysy�any pakiet utrzymania po��czenia z serwerem
sendPingTime = 60000