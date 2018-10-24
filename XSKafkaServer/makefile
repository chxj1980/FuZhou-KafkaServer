lib = -lpthread -lzmq -lrdkafka -lmysqlclient -lhiredis -I/usr/local/include/mysql -L/usr/local/lib/mysql
objects = XSKafkaServer.o KafkaServer.o KafkaProducer.o KafkaConsumer.o ZeromqManage.o RedisManage.o ConfigRead.o
target=XSKafkaServer

$(target): $(objects)
	g++ -o $(target) $(objects) $(lib)
XSKafkaServer.o: XSKafkaServer.cpp KafkaServer.h
	g++ -c  XSKafkaServer.cpp 
KafkaServer.o: KafkaServer.cpp KafkaServer.h DataDefine.h
	g++ -c KafkaServer.cpp
KafkaProducer.o: KafkaProducer.cpp KafkaProducer.h
	g++ -c KafkaProducer.cpp -I/usr/local/include/librdkafka 
KafkaConsumer.o: KafkaConsumer.cpp KafkaConsumer.h
	g++ -c KafkaConsumer.cpp
ZeromqManage.o: ZeromqManage.cpp ZeromqManage.h DataDefine.h
	g++ -c ZeromqManage.cpp
RedisManage.o: RedisManage.cpp RedisManage.h
	g++ -c RedisManage.cpp
ConfigRead.o: ConfigRead.cpp ConfigRead.h
	g++ -c ConfigRead.cpp
	

clean:
	rm $(objects)

cpp:
	rm *.cpp *.h
