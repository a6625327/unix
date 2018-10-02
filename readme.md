# ******************* 文档说明 *******************
#
# 本文档包括以下内容：
# 一、基本环境部署
#   1）基本软件安装
#   2）日志系统部署
#   3）配置文档解析系统部署
#   4）编译链接本程序
#
# 二、创建热点基本依赖环境部署
#
# ************************************************
#
# 本程序路径假定为： ～/project/unix
#
# 一、 ---------------- 基本环境部署 ---------------
# 1）基本软件安装
sudo apt-get update
sudo apt-get install gcc g++ gdb cgdb make cmake git 

# 2）日志系统部署
#    本程序采用zlog作为日志系统
#    zlog主页、帮助文档：http://hardysimpson.github.io/zlog/
#    zlog的git：https://github.com/lisongmin/zlog
#    将源码包下载至本地:
git clone https://github.com/lisongmin/zlog.git
cd zlog
sudo make
sudo make install 

# 3）配置文档解析系统部署
#    本程序采用iniparser作为配置文档解析系统，本程序编译链接采用动态方式引入第三方库 
#    iniparser的git：https://github.com/ndevilla/iniparser
#    将源码包下载至本地：
git clone https://github.com/ndevilla/iniparser.git
cd iniparsers
sudo make
#
#    然后将生成的动态库拷贝至本程序目录下的libs/iniparser即可(编译链接本程序时，会执行脚本完成动态库的自动部署)
cp libiniparser.so.1 ~/project/unix/libs/iniparser
#    将src下的头文件拷贝至本程序头文件库中
mkdir ~/project/unix/include/iniparser
cp src/*.h ~/project/unix/include/iniparser
#
# -------------------------------------------------------------

# 4）编译链接本程序
cd ~/project/unix
sh cmake.sh
# 可执行程序会生成在bin文件夹下
#
# 如果需要清除编译链接生成的中间文件、日志文件、传输产生的缓存文件、可执行文件，则执行：
bash clean.sh

# 二、 ------------------ 创建热点基本依赖环境部署 -----------------
# 1、安装 hostapd
sudo apt-get install hostapd

# 2、安装dhcp server
sudo apt-get install isc-dhcp-server

# 其原理如下：
# 1)、开启hostapd让电脑的无线网卡工作在Master模式，开启后，其他设备能够搜索到该热点，但由于无法获取到IP地址仍然无法连接。
# 根据实际情况，选择采用网桥方式或是DHCP服务器加上NAT来使该AP可用。
# 2)、如果选择的是网桥方式，那么IP地址将会由电脑实际连接的外网分配，并且连接的终端可以直接发送数据到外网。热点连上了就可以直接用了，且和计算机的有线端同处于一个子网。
# 3)、如果选择的是DHCP服务器加NAT的方式，那么开启DHCP服务器，就能够连上热点，但是无法连到网络。通过配置NAT，让终端发出的数据包的IP修改为公网IP，即可连上网络。

# 3、配置isc-dhcp-server
# 此处为一个demo
sudo vim /etc/dhcp/dhcpd.conf
# 在最后面添加一下配置（该段摘自官方文档）
# ==================================================
#   subnet即子网段；range为给子网分配ip时的范围；domain-name*为域名服务器；routers为子网所走的路由；
    subnet 10.5.5.0 netmask 255.255.255.224 {
        range 10.5.5.26 10.5.5.30;
        option domain-name-servers ns1.internal.example.org;
        option domian-name "internal.example.org";
        option routers 10.5.5.1;
        option broadcast-address 10.5.5.31;
        default-lease-time 600;
        max-lease-time 7200;
    }
# ==================================================

# 4、配置wlan0需要dhcp服务
sudo vim /etc/default/isc-dhcp-server
# 修改以下内容：
# ==================================================
    INTERFACESv4="wlan0"
# ==================================================

# 5、需要修改无线网卡的ip，使之与dhcp服务器所指定的ip网段、ip一致
sudo vim /etc/network/interfaces
# 添加以下内容：
# ==================================================
    auto wlan0
    iface wlan0 inet static
    address 10.5.5.1
    netmask 255.255.255.0
    broadcast 10.5.5.255
    gateway 10.5.5.1
# ==================================================
# 保存，然后重启网络服务
sudo /etc/init.d/networking restart

# 6、重启dhcp服务，使之配置对无线网卡wlan0生效
sudo /etc/init.d/isc-dhcp-server start

# 7、启动hostapd即可创建软AP，配置好hostapd的配置文件hostapd.conf，然后在终端输入以下命令即可启动，其中xx/xxx是你存放对应配置文件的位置。
sudo hostapd xx/xxx/hostapd.conf
# 下面是一个配置demo
# ==================================================
    #无线设备名称，基本都是wlan0，可用iwconfig命令查看
    interface=wlan0

    #使用的网桥名称，如果是用DHCP+NAT方式可忽略此行
    #bridge=br0  

    #hostapd 0.6.8或者更高版本唯一选择
    driver=nl80211

    #终端看到的wifi名称，请自行修改
    ssid=mywifi

    #指明要选用的无线传输协议，这里表示使用802.11g
    hw_mode=g

    #802.11b/g都至多只有三条互不干扰的信道，即1,6,11，一般填这三个其中一个    
    channel=1 

    #验证身份的算法，1表示只支持wpa，2表示只支持wep,3表示两者都支持，wep已经被淘汰了，请不要使用。
    auth_algs=1

    #wpa加密方式，1代表支持wpa,2代表支持wap2,3代表两者都支持。
    wpa=1

    #wifi密码，请自行修改
    wpa_passphrase=123456789

    #对所有用户进行同样的认证，不进行单独的认证，如果需要，请设置为WPA-EAP。
    wpa_key_mgmt=WPA-PSK

    #控制支持加密数据的秘钥，CCMP比TKIP更强
    wpa_pairwise=CCMP
# ==================================================
#
# --------------------------------------------------
