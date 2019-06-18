### Add a reset function in LoraMessage  
in LoraMessage.cpp add these lines at the bottom:  

```
void LoraMessage::reset(){
	_currentSize = 0;
}
```
Aslo in LoraMessage.h add  
```
void reset();

```  
  
  error "bad memory allocation!"  
  check the output data for message.addUnixtime ior .add16bit  
  the reason is due to overflow  
  If you want to use 32bit, you can use .addUnixtime()
