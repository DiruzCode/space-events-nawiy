



# space-events-nawiy
spaces events Ñawiy

### Install NodeJS 6.x

```bash
$ sudo yum update -y
$ sudo yum install -y gcc gcc-c++ make openssl-devel
$ curl --silent --location https://rpm.nodesource.com/setup_6.x | bash -
$ yum -y install nodejs
```

### Install PHP7 and HTTPD



```bash
$ sudo yum update -y
$ sudo yum install -y httpd24 php70 mysql56-server php70-mysqlnd
```

```bash
$ sudo service httpd start
Starting httpd:                                            [  OK  ]
```


Tip
The chkconfig command does not provide any confirmation message when you successfully use it to enable a service.
You can verify that httpd is on by running the following command:

```bash
$ sudo chkconfig httpd on
httpd           0:off   1:off   2:on    3:on    4:on    5:on    6:off

$ sudo yum update -y
$ php -S 0.0.0.0:PORT
```



### Clone

```bash
$ git clone https://github.com/DiruzCode/space-events-nawiy.git
```


### API

cd space-events-nawiy/API

```bash
$ npm install
```

Create File .env 

DB_HOST=
DB_USER=
DB_PASS=


```bash
$ forever start server.js
```

### FacebookBot

cd space-events-nawiy/FacebookBot

```bash
$ npm install
```

Create File .env 

PAGE_ACCESS_TOKEN=
DB_HOST=
DB_USER=
DB_PASS=


```bash
$ forever start server.js
```

### Client

cd space-events-nawiy/Client

```bash
$ nohup php -S 0.0.0.0:PORT &
```

### Ngrok

you need a ssh tunel if you haven't SSL Certificate / HTTPS

This is only for use facebook bot

Install https://ngrok.com

./ngrok 0.0.0.0:5000


If you don't know who created a bot, you can use this tutorial http://www.girliemac.com/blog/2017/01/06/facebook-apiai-bot-nodejs/
