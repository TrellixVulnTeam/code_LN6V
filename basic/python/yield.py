#encoding:UTF-8  
def yield_test(n):  
    for i in range(n):  
        yield call(i)     # ����������call(i)������ɹ��ó�����Ż������һ�������� i, ',' ����ִ��
        print("function i=",i)     # ��ִ��
    #��һЩ����������      
    print("do something.")        #  ��ִ�У�����ִ��һ��
    print("end.")  
  
def call(i):  
    return i*2  
  
#ʹ��forѭ��  
for i in yield_test(5):  
    print( "Return i = " , i)   # ����� i �� call(i)
