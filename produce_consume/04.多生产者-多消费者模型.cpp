#include <unistd.h>

#include <cstdlib>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

static const int kItemRepositorySize  = 4; // Item buffer size.
static const int kItemsToProduce  = 10;   // How many items we plan to produce.

struct ItemRepository {
	int item_buffer[kItemRepositorySize];
	size_t read_position;
	size_t write_position;
	size_t produced_item_counter;
	size_t consumed_item_counter;
	std::mutex mtx;
	std::mutex produced_item_counter_mtx;
	std::mutex consumed_item_counter_mtx;
	std::condition_variable repo_not_full;
	std::condition_variable repo_not_empty;
} gItemRepository;

typedef struct ItemRepository ItemRepository;


void ProduceItem(ItemRepository *ir, int item)
{
	std::unique_lock<std::mutex> lock(ir->mtx);
	while(((ir->write_position + 1) % kItemRepositorySize)
		  == ir->read_position) { // item buffer is full, just wait here.
		std::cout << "Producer is waiting for an empty slot...\n";
		(ir->repo_not_full).wait(lock);
	}

	(ir->item_buffer)[ir->write_position] = item;
	(ir->write_position)++;

	if (ir->write_position == kItemRepositorySize)
		ir->write_position = 0;

	(ir->repo_not_empty).notify_all();
	lock.unlock();
}

int ConsumeItem(ItemRepository *ir)
{
	int data;
	std::unique_lock<std::mutex> lock(ir->mtx);
	// item buffer is empty, just wait here.
	while(ir->write_position == ir->read_position) {
		std::cout << "Consumer is waiting for items...\n";
		(ir->repo_not_empty).wait(lock);
	}

	data = (ir->item_buffer)[ir->read_position];
	(ir->read_position)++;

	if (ir->read_position >= kItemRepositorySize)
		ir->read_position = 0;

	(ir->repo_not_full).notify_all();
	lock.unlock();

	return data;
}

void ProducerTask()
{
	bool ready_to_exit = false;
	while(1) {
		sleep(1);
		std::unique_lock<std::mutex> lock(gItemRepository.produced_item_counter_mtx);
		if (gItemRepository.produced_item_counter < kItemsToProduce) {
			++(gItemRepository.produced_item_counter);
			ProduceItem(&gItemRepository, gItemRepository.produced_item_counter);
			std::cout << "Producer thread " << std::this_thread::get_id()
				<< " is producing the " << gItemRepository.produced_item_counter
				<< "^th item" << std::endl;
		} else ready_to_exit = true;
		lock.unlock();
		if (ready_to_exit == true) break;
	}
	std::cout << "Producer thread " << std::this_thread::get_id()
		<< " is exiting..." << std::endl;
}

void ConsumerTask()
{
	bool ready_to_exit = false;
	while(1) {
		sleep(1);
		std::unique_lock<std::mutex> lock(gItemRepository.consumed_item_counter_mtx);
		if (gItemRepository.consumed_item_counter < kItemsToProduce) {
			int item = ConsumeItem(&gItemRepository);
			++(gItemRepository.consumed_item_counter);
			std::cout << "Consumer thread " << std::this_thread::get_id()
				<< " is consuming the " << item << "^th item" << std::endl;
		} else ready_to_exit = true;
		lock.unlock();
		if (ready_to_exit == true) break;
	}
	std::cout << "Consumer thread " << std::this_thread::get_id()
		<< " is exiting..." << std::endl;
}

void InitItemRepository(ItemRepository *ir)
{
	ir->write_position = 0;
	ir->read_position = 0;
	ir->produced_item_counter = 0;
	ir->consumed_item_counter = 0;
}

int main()
{
	InitItemRepository(&gItemRepository);
	std::thread producer1(ProducerTask);
	std::thread producer2(ProducerTask);
	std::thread producer3(ProducerTask);
	std::thread producer4(ProducerTask);

	std::thread consumer1(ConsumerTask);
	std::thread consumer2(ConsumerTask);
	std::thread consumer3(ConsumerTask);
	std::thread consumer4(ConsumerTask);

	producer1.join();
	producer2.join();
	producer3.join();
	producer4.join();

	consumer1.join();
	consumer2.join();
	consumer3.join();
	consumer4.join();
}
/*
Producer thread Consumer thread 139884361373440139884336195328 is producing the 1 is consuming the ^th item1^th item

Producer thread 139884352980736 is producing the 2^th item
Producer thread 139884369766144Consumer thread 139884327802624 is consuming the 2^th item is producing the 
3^th item
Consumer thread 139884319409920 is consuming the 3^th item
Producer thread 139884344588032 is producing the 4^th item
Consumer thread 139884311017216 is consuming the 4^th item
Producer thread 139884361373440 is producing the 5^th item
Consumer thread 139884336195328 is consuming the Producer thread 5^th item
Consumer thread 139884319409920 is consuming the 6^th item139884352980736
 is producing the 6^th item
Consumer is waiting for items...
Producer thread 139884369766144 is producing the 7^th item
Consumer thread 139884327802624 is consuming the 7^th item
Producer thread 139884344588032 is producing the 8^th item
Consumer thread 139884311017216 is consuming the 8^th item
Producer thread 139884361373440 is producing the 9^th item
Consumer thread 139884336195328 is consuming the 9^th item
Consumer is waiting for items...
Producer thread 139884352980736 is producing the 10^th itemConsumer thread 
139884319409920 is consuming the Producer thread 10^th item139884369766144
 is exiting...
Producer thread Consumer thread 139884327802624 is exiting...
139884344588032 is exiting...
Consumer thread 139884311017216 is exiting...
Producer thread 139884361373440 is exiting...
Consumer thread 139884336195328 is exiting...
Producer thread 139884352980736 is exiting...
Consumer thread 139884319409920 is exiting...
*/
