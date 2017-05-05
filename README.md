# space-events-nawiy
spaces events Ñawiy



## Clone

```bash
$ git clone https://github.com/DiruzCode/space-events-nawiy.git
```


### API

cd space-events-nawiy/API

```bash
$ npm install
```
```bash
$ forever start server.js
```

### FacebookBot

cd space-events-nawiy/FacebookBot

```bash
$ npm install
```
```bash
$ forever start server.js
```

### Client

cd space-events-nawiy/Client

```bash
$ nohup php -S 0.0.0.0:PORT &
```

## Install PHP7 and HTTPD



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
