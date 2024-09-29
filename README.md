use inetd to create a tcp connection and for forking
after that vpnServer is invoked
vpnServer is supposed to use the tcp connection to authenticate users
after authenticating users, vpnServer closes the tcp connection and then starts a UDP connection on another port

inetd files:
/etc/services
/etc/inetd.conf



gcc -pthread vpnServerForMultipleClientsPlusMysqlAndScriptsAndInetdAndCryptography.c -o vpnServer -lcrypto

tail -f '/home/hp/Desktop/androidVpnServerLog'


Disable ufw after modifying /etc/ufw/before.rules FOR IP MASQUERADING



IP MASQUERADING COMMANDS for internet browsing [use ip address of remote endpoint as the source]
sudo iptables -t nat -A POSTROUTING -s 10.0.0.10 -o enp7s0 -j MASQUERADE

sudo iptables -t nat -L

I THINK THERE'S A BUG IN THE LINUX WIFI HOTSPOT CODE COZ SOMETIMES IT AUTOMATICALLY DISCONNECTS even when
i'm using it for purposes other than a vpn (so, my  android phone should host a wi-fi hotspot when testing the app. )

BOTH LINUX SERVER APP AND ANDROID APP ARE NOT CRUSHING EVEN THOUGH SOMETIMES DATA SENT IS ABOVE MTU.









gcc -c `mysql_config --cflags` vpnServerForMultipleClientsPlusMysqlAndScriptsAndInetdAndCryptography.c

gcc -o vpnServer vpnServerForMultipleClientsPlusMysqlAndScriptsAndInetdAndCryptography.o `mysql_config --libs`


i think wifi (including the hotspot) turns off when Android phones go on standby therefore the connection message will not
be sent. Listen for android.content.Intent ACTION_SCREEN_OFF















