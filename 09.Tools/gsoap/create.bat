@rem -s��ʹ��STL���� -oָ������ļ� -t mytypema.dat�������ַ�������  WebService��ַ

wsdl2h.exe  -s -o Service.h http://www.webxml.com.cn/WebServices/ChinaStockWebService.asmx
soapcpp2.exe Service.h