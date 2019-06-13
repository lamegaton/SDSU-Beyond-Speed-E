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
