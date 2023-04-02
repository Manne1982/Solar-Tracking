#ifndef HTML_Var
#define HTML_Var


#include <pgmspace.h>
#include <Arduino.h>

//Definitionen fuer Datum und Uhrzeit
// enum {clh, clmin}; //clh ist 0 fuer Uhrzeit-Stunden, clmin ist 1 fuer Uhrzeit Minuten
// enum {dtYear, dtMonth, dtDay}; //Tag = 2; Monat = 1; Jahr = 0
const String WeekDays[7]={"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
// //Allgemeine Definitionen
enum {subwl = 27767, subnw = 30574, subcn = 20035, subpd = 17488, subps = 21328, sublf = 17996, sublm = 19788, submq = 29037}; //Zuordnung der Submit-Bereiche einer Ganzzahl
const String Un_Checked[2]{"","Checked"};
const String varSelected[2]{"", " selected=\"\""};
// const String De_Aktiviert[2]{"Deaktiviert","Aktiviert"};
//const String Ein_Aus[2]{"Aus","Ein"};
// unsigned long BaudrateRelais = 9600;
// unsigned long BaudrateDebug = 115200;

//Menünamen                 0               1                 2                   3                 4               5               6                7               8          9              10           11          12              13            14            15            16      17            18              19        20            21          22         23         24        25            26
//const String MenuName[30]={"Allgemein", "Wasser St l", "Verbr. Ges m3", "Verbr. ml/sek", "Wasser Dif T", "Wasser Dif W", "Wasser Dif M", "Wasser St", "Wasser St Roh", "LED-Config", "Modus", "Speed", "Standard Dauer", "Kanalname", "Offset in %", "Dauer in %", "NW-Config", "Accesspoint", "SSID", "Statische IP", "IP-Adresse", "NW-Name", "Subnetmask", "Gateway", "DNS-Server", "Zeitserver", "Zeitoffset"};
//const String LEDMode[9]={"Farbe", "Rot", "Gruen", "Blau", "Weiss", "Flash", "Strobe", "Fade", "Smoth"};


const char html_header[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Solar Nachführung</title>
  <meta name="viewport" content="width=device-width, initial-scale=1", charset="UTF-8">
</head>
<body bgcolor=\"#BBFFFF\">
Uhrzeit: %s | Datum: %s, %02d.%02d.%d
<br />
<hr><h3>
<a href=\>Startseite</a> | 
<a href=\Settings\>Einstellungen</a> | 

</h3><hr>
)rawliteral";
const char html_Start[] PROGMEM = R"rawliteral(
<h1>Solar Nachführung Startseite</h1><hr>
<h2>Informationen</h2><br />
<TABLE border="1">
  <TR>
    <TD WIDTH="250" VALIGN="TOP">
      Startposition<br /></TD>
    <TD WIDTH="100" VALIGN="TOP">
	  &nbsp %u</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Endposition<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %u</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Endanschlag<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %u</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Aktuelle Position<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %u</TD>
  </TR>
</TABLE>
<br />


<h2>Zusatzfunktionen</h2><br />
<TABLE> <!-- 'border="1"-->
  <TR>
    <TD WIDTH="220" VALIGN="TOP">
      <a href=/DisplayOn>Display einschalten</a><br /></TD>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DisplayOff>Display ausschalten</a><br /></TD>
  </TR>
</TABLE>
<br />
</body>
</html>
)rawliteral";


const char html_NWconfig[] PROGMEM = R"rawliteral(
<h1>Heizung Zusatzsteuerung NW Einstellungen</h1><hr>
<h2>WLAN</h2>
<form method="post" action="/POST">
<TABLE>
  <TR>
    <TD WIDTH="120" VALIGN="TOP">
    Access Poin: </TD>
    <TD WIDTH="300" VALIGN="TOP">
    <input name="wlAP" value="1" type="checkbox" %s> <br /><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      SSID: </TD>
  <TD>
    <input name="wlSSID" type="text" value="%s" minlength="2" maxlength="30" size="15" required="1"><br /><br /></TD>
  <TD>
    <br /><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Passwort: </TD>
  <TD>
    <input name="wlPassword" type="password" value="xxxxxx" minlength="8" maxlength="60" size="35"><br /><br /></TD>
  </TR>
</TABLE>
  <BR>
  <input value="Submit" type="submit">
</form>
<hr>
<h2>Netzwerk</h2><br />
<form method="post" action="/POST">
<TABLE>
  <TR>
    <TD WIDTH="200" VALIGN="TOP">
      Ethernet on: </TD>
    <TD WIDTH="200" VALIGN="TOP">
    <input name="nwEthernetOn" value="" type="checkbox"> <br /><br /></TD>
    <TD WIDTH="200" VALIGN="TOP">
    </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Manuelle IP Konfiguration: </TD>
    <TD VALIGN="TOP">
    <input name="nwSIP" value="" type="checkbox"> <br /><br /></TD>
    <TD VALIGN="TOP">
    </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      IP-Adresse: </TD>
  <TD>
      <input name="nwIP" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
       </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      IP-Adresse Ethernet: </TD>
  <TD>
      <input name="nwIPEthernet" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
       </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Netzwerkname: </TD>
  <TD>
    <input name="nwNetzName" type="text" minlength="3" maxlength="15"  value="%s" required="1"> <br /><br /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Subnetmask: </TD>
  <TD>  
    <input name="nwSubnet" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
       </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Gateway: </TD>
  <TD>
    <input name="nwGateway" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
      Wird nur bei einem externen NTP-Server benoetigt!</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      DNS-Server: </TD>
  <TD>
    <input name="nwDNS" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
      Wird nur bei einem externen NTP-Server benoetigt!</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Zeitserver (z.B. fritz.box): </TD>
  <TD>
    <input name="nwNTPServer" type="text" minlength="3" maxlength="50"  value="%s" required="1"> <br /><br /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Zeitoffset (in Stunden): </TD>
    <TD>
      <select name="nwNTPOffset" autofocus="autofocus">
      <option%s>-2</option>
      <option%s>-1</option>
      <option%s>0</option>
      <option%s>+1</option>
      <option%s>+2</option>
      </select> 
    <BR /><BR /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>

</TABLE>
    <BR>
  <input value="Submit" type="submit">
  </form>
<HR>
<h2>MQTT</h2><BR>
<form method="post" action="/POST">
<TABLE>
  <TBODY>
  <TR>
    <TD VALIGN="TOP">
      MQTT On/Off: </TD>
    <TD VALIGN="TOP">
    <input name="mqMQTTOn" value="" type="checkbox" %s> <br /><br /></TD>
  </TR>
  <TR>
    <TD valign="TOP">
      MQTT Server: </td>
    <TD>
      <input name="mqServer" type="text" minlength="7" maxlength="15" size="45" value="%s"><br><br></td>
    <TD valign="TOP">
       </td>
  </TR>
  <TR>
    <td valign="TOP">
      MQTT Port: </td>
    <TD>
    <input name="mqPort" type="number" minlength="3" maxlength="5" size="8" value="%u" required="1" pattern="[0-9]{5}"> <br><br></td>
    <TD valign="TOP">
      </td>
  </TR>
  <TR>
    <TD valign="TOP">
      Benutzername: </td>
    <TD>  
    <input name="mqUser" type="text" minlength="6" maxlength="15" size="19" value="%s"><br><br></td>
    <TD valign="TOP">
       </td>
  </TR>
  <TR>
    <TD valign="TOP">
      Passwort: </td>
    <TD>
    <input name="mqPassword" type="password" minlength="5" maxlength="60" size="35" value="xxxxxx"><br><br></td>
    <TD valign="TOP">
  </TR>
  <TR>
    <TD valign="TOP">
      MQTT Hauptpfad: </td>
  <TD>
    <input name="mqRootpath" type="text" minlength="5" maxlength="100" size="35" value="%s"><br><br></td>
    <TD valign="TOP">
  </TR>
  <TR>
    <TD VALIGN="TOP">
      MQTT secure On/Off: </TD>
    <TD VALIGN="TOP">
    <input name="mqMQTTsecureOn" value="" type="checkbox" %s> <br /><br /></TD>
  </TR>
  <TR>
    <TD valign="TOP">
      Zertifikat: </td>
    <TD>
    <input name="mqFPrint" type="password" minlength="8" maxlength="60" size="35" value="xxxxxx"><br><br></td>
    <TD valign="TOP">
  </TR>

</tbody></table>
    <br>
  <input value="Submit" type="submit">
  </form>

</body>
</html>
)rawliteral";

#endif