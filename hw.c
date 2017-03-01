/* Hello World program */

#include<stdio.h>
#include<thread>
#include<iostream>
void call_from_thread()
{
	std::cout << "Hello World" << std::endl;
}
int main()
{
	for(int i=0; i<5; i++)
	{
		std::thread t(call_from_thread);
		t.join();
	}
	
	return 0;
}
