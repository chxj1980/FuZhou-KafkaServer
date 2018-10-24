lib = -lpthread -lzmq -lrdkafka 
objects = XSKafkaServer.o KafkaServer.o KafkaProducer.o KafkaConsumer.o ZeromqManage.o ConfigRead.o
target=XSKafkaConsumer

$(target): $(objects)
	g++ -o $(target) $(objects) $(lib)
XSKafkaServer.o: XSKafkaServer.cpp KafkaServer.h
	g++ -c  XSKafkaServer.cpp 
KafkaServer.o: KafkaServer.cpp KafkaServer.h DataDefine.h
	g++ -c KafkaServer.cpp
KafkaProducer.o: KafkaProducer.cpp KafkaProducer.h
	g++ -c KafkaProducer.cpp -I/usr/local/include/librdkafka
KafkaProducer.o: KafkaProducer.cpp KafkaProducer.h
	g++ -c KafkaProducer.cpp 
KafkaConsumer.o: KafkaConsumer.cpp KafkaConsumer.h
	g++ -c KafkaConsumer.cpp
ZeromqManage.o: ZeromqManage.cpp ZeromqManage.h DataDefine.h
	g++ -c ZeromqManage.cpp
ConfigRead.o: ConfigRead.cpp ConfigRead.h
	g++ -c ConfigRead.cpp
	

clean:
	rm $(target) $(objects)

cpp:
	rm *.cpp *.h
