#include <windows.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <chrono>
#include <ctime>
 HANDLE full_semaphore;
 HANDLE empty_semaphore;
 int in = 0;
 int out = 0;
 int count = 0;
 int object = 0;
DWORD Buffer[5];
std::mutex mtx;
void Consumer()
{
	while (true) 
	{
		WaitForSingleObject(full_semaphore, INFINITE);  
		count--;
		if (count == 0) 
			printf("Потребитель считал объект %d из %d-го элемента буфера. Буфер пуст, потребитель ждёт появления новых объектов.\n", Buffer[out], out);
		else
			printf("Потребитель считал объект %d из %d-го элемента буфера.\n", Buffer[out], out);
		out = (out + 1) % 5; 
		ReleaseSemaphore(empty_semaphore, 1, NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(rand() %4999));
	}
}
int main()
{
	srand(time(NULL));
	std::thread t1(Consumer);
	setlocale(LC_ALL, "Russian");
	int i;
	full_semaphore = CreateSemaphore(NULL, 0, 5, TEXT("full")); 
	empty_semaphore = CreateSemaphore(NULL, 5, 5, TEXT("empty"));
	for (i = 0; i < 5; i++)
	{
		Buffer[i] = 0;
	}
	while (true) 
	{
		WaitForSingleObject(empty_semaphore, INFINITE); 
		mtx.lock();
		Buffer[in] = object; object++; 
		count++;
		if (count >= 5) 
			printf("Производитель поместил объект %d в %d-й элемент буфера. Буфер заполнен, производитель ждёт освобождения места.\n", Buffer[in], in);
		else
			printf("Производитель поместил объект %d в %d-й элемент буфера.\n", Buffer[in], in);
		in = (in + 1) % 5; 
		mtx.unlock();
		ReleaseSemaphore(full_semaphore, 1, NULL);
		std::this_thread::sleep_for(std::chrono::milliseconds(rand()%4999));
	}
	CloseHandle(full_semaphore);
	CloseHandle(empty_semaphore);
	
	
	t1.join();
	return 0;
}
