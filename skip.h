#ifndef __SKIP_LIST_H__
#define __SKIP_LIST_H__

#define DEBUG 1

#include <functional>
#include <memory>
#include <limits>
#include <cstdlib>
#include <unistd.h> // for rand()

#if DEBUG
#include <iostream>
#include <vector>
#include <algorithm>
#endif

/*
	See: http://www.mathcs.emory.edu/~cheung/Courses/323/Syllabus/Map/FIGS/skip-list22.gif
*/

template<
		typename T,
		typename Compare = std::less<T>,
		typename Allocator = std::allocator<T>
		>
class skip_list {
public:

	// Member types
	using allocator_type   =  Allocator;
	using value_type       =  typename std::allocator_traits<allocator_type>::value_type;
	using pointer          =  typename std::allocator_traits<allocator_type>::pointer;
	using reference        =  value_type&;
	using const_pointer    =  typename std::allocator_traits<Allocator>::const_pointer;
	using const_reference  =  const value_type&;
	using size_type        =  std::size_t;
	
private:
	const size_type h; // max height of the skip list - doesn't change for now
	size_type n; // counter for number of nodes in the list

	allocator_type alloc = allocator_type();
	Compare comp;

	// Node definition
	struct node {

		//make this pointer to save data
		pointer data;
		// pointer to its adjacent nodes
		node *next;
		node *prev;
		node *top;
		node *bottom;
		
		//constructor
		explicit node( pointer data = nullptr,
					   node *next = nullptr, 
					   node *prev = nullptr, 
					   node *top = nullptr, 
					   node *bottom = nullptr )
		: data(data), next(next), prev(prev), top(top), bottom(bottom) {

		}
		
		node(node &other) {
			this->data = other.data;
			this->top = other.top;
			this->bottom = other.bottom;
			this->left = other.left;
			this->right = other.right;
		}

		node& operator=(const node& other) {
			this->data = other.data;
			this->top = other.top;
			this->bottom = other.bottom;
			this->left = other.left;
			this->right = other.right;
			return *this;
		}

		//utility functions
		bool is_head() const {
			return this->prev == nullptr && this->data == nullptr;
		}

		bool is_tail() const {
			return this->next == nullptr && this->data == nullptr;
		}

		static bool compare_me(	const skip_list<T, Compare, Allocator> *o, 
								const node& lhs, 
								const node& rhs) {
			if(lhs.is_head()) {
				return true;
			}
			if(lhs.is_tail()) {
				return false;
			}	
			return o->comp(*(lhs.data),*(rhs.data));
		}
	};

	//pointers to head and tail
	node *head;
	node *tail; //exclusive
	
	bool should_propogate(){
		return std::rand() % 2 == 1;	
	}

public:

	skip_list(size_type h = 1)
	: h(h), n(0) {
		
		std::srand(getpid()); // Purely randomness
	
		// make a stack of head and tail nodes about ye (h) high
		
		head = new node(); //Dummy Head node
		tail = new node(); //Dummy Tail node
		//init head and tails (empty skiplist)
		head->next = tail;
		tail->prev = head;

		node *h_stack = head;
		node *t_stack = tail;

		for(size_type i = 1; i < h; ++i) {
			h_stack -> bottom = new node();
			h_stack -> bottom -> top = h_stack;
			
			t_stack -> bottom = new node();
			t_stack -> bottom -> top = t_stack;
			
			h_stack -> bottom -> next = t_stack -> bottom;
			t_stack -> bottom -> prev = h_stack -> bottom;
			
			h_stack = h_stack -> bottom;
			t_stack = t_stack -> bottom;
		}
	}

	~skip_list() {
		// Logic:
		// Iterate through each level and delete the nodes across the level
		node *next_level = nullptr;
		for(size_type i = 0; i < h-1; ++i) {
			next_level = head -> bottom;
			while(head) {
				node *temp = head;
				head = head -> next;
				delete temp;
			}
			head = next_level;
		}
		if(next_level) {
			node *temp = next_level->next;
			delete next_level;
			next_level = temp;
			while(next_level->next) {
				node *temp = next_level->next;
				alloc.deallocate(next_level->data, 1);
				delete next_level;
				next_level = temp;
			}
			delete next_level;
			head = nullptr;
			tail = nullptr;
		}
	}

	skip_list(const skip_list<T, Compare, Allocator> &other) = delete;
	skip_list operator=(const skip_list<T, Compare, Allocator> &other) = delete;
	
	skip_list(const skip_list<T, Compare, Allocator> &&other) = delete;
	skip_list operator=(const skip_list<T, Compare, Allocator> &&other) = delete;
	
	void insert(const_reference data){
		node *mynode = new node(alloc.allocate(1));
		*(mynode -> data) = data;
		
		node *curr_ptr = head;
		
		// Finding pos to insert
		while( curr_ptr -> bottom != nullptr){
			if( !( node::compare_me(this, *(curr_ptr -> next), *mynode) ) ){
				curr_ptr = curr_ptr -> bottom;
			}
			else{
				curr_ptr = curr_ptr -> next;
			}
		}
		while(node::compare_me(this, *(curr_ptr -> next), *mynode)){
				curr_ptr = curr_ptr -> next;
		}
		
		mynode -> next = curr_ptr -> next;
		mynode -> prev = curr_ptr;
		mynode -> next -> prev = mynode;
		curr_ptr -> next = mynode;
		
		// Adding node in upper level - if at all it is lucky
		while(should_propogate()){ /*true*/
			// checking for node which has propogated
			while(curr_ptr != nullptr && curr_ptr -> top == nullptr){
				curr_ptr = curr_ptr -> prev;
			}
			// checking if it is in the top level
			if(curr_ptr == nullptr) {
				break;
			}
			// Inserting new node (mynode) in the current level
			curr_ptr = curr_ptr -> top;
			node *level_mynode = new node(mynode -> data);
			level_mynode -> next = curr_ptr -> next;
			level_mynode -> prev = curr_ptr;
			level_mynode -> next -> prev = level_mynode;
			level_mynode -> bottom = mynode;
			level_mynode -> bottom -> top = level_mynode;
			curr_ptr -> next = level_mynode;
			mynode = level_mynode;
		}
	}
	
	#if DEBUG
	void pretty_print() {
		
		std::vector<std::vector<value_type>> idk;
		node *temp = head;
		
		
		while(temp -> bottom){
			temp = temp -> bottom;
		}
		while(temp){
			std::vector<value_type> level_vector;
			node *temp_temp = temp->next;
			while(temp_temp && !temp_temp->is_tail()){
				level_vector.push_back(*(temp_temp->data));
				temp_temp = temp_temp -> next;
			}
			//std::reverse(level_vector.begin(), level_vector.end());
			idk.push_back(level_vector);
			temp = temp -> top; 
		}
		
		#if 0
		std::for_each(idk.begin(), idk.end(), [](auto e){
			std::for_each(e.begin(), e.end(), [](auto v){
				std::cout << v << "->";
			});
			std::cout << "\n";
		});
		#endif 
		

		for(int i = h-1; i > 0; --i) {
			//iterates through all level vectors other than base
			auto base_it = idk[0].begin();
			auto curr_it = idk[i].begin();
			std::cout << "[h]\t";
			if(curr_it == idk[i].end()){
				while(base_it != idk[0].end()) {
					std::cout << "-" << "\t";
					++base_it;
				}
			}
			else{
				while(base_it != idk[0].end()) {
				
					//if(*base_it == *curr_it) {
					if((!comp(*base_it, *curr_it) && !comp(*curr_it, *base_it))) {
						std::cout << *base_it << "\t";
						++base_it;
						++curr_it;
					}
					else {
						std::cout << "-\t";
						++base_it;
					}
				}
			}
			std::cout << "[t]\t";
			std::cout << std::endl;
		}
		auto base_it = idk[0].begin();
		std::cout << "[h]\t";
		while(base_it != idk[0].end()) {
			std::cout << *base_it++ << "\t";
		}
		std::cout << "[t]\t";
		std::cout << std::endl;
		
	}
	#endif
	

	
	class Iterator{
	public:
		node *curr;
		
		Iterator(skip_list<T, Compare, Allocator>* l) : curr(l->head) {
			while(curr->bottom) {
				curr = curr->bottom;
			}
			curr = curr->next;
		}
		
		Iterator(const Iterator& cp) {
			this->curr = cp.curr;
		}
		
		Iterator operator=(const Iterator &cp) {
			this->curr = cp.curr;
		}
		
		Iterator operator++() { //prefix
			curr = curr->next;
			return *this;
		}
	
		Iterator operator++(int) { //postfix
			Iterator temp = *this;
			++(*this);
			return temp;
		}
		
		Iterator operator--() { //prefix
			curr = curr->prev;
			return *this;
		}
	
		Iterator operator--(int) { //postfix
			Iterator temp = *this;
			--(*this);
			return temp;
		}
		
		reference operator*() const { //dereference
			return *(curr->data);
		}
		
		friend bool operator==(Iterator& lhs, Iterator& rhs) {
			return lhs.curr == rhs.curr;
		}
		
		friend bool operator!=(Iterator& lhs, Iterator& rhs) {
			return !(lhs == rhs);
		}
		
	};
	
	Iterator begin() {
		return Iterator(this);
	}
	
	Iterator end() {
		auto tempI = Iterator(this);
		while(!tempI.curr->is_tail()) {	
			tempI.curr = tempI.curr->next;
		}
		return tempI;
	}
	
	Iterator find(const_reference data) {
		node *mynode = new node(alloc.allocate(1));
		*(mynode -> data) = data;
		
		node *curr_ptr = head;
		
		// Finding pos to insert
		while( curr_ptr -> bottom != nullptr){
			if( !( node::compare_me(this, *(curr_ptr -> next), *mynode) ) ){
				curr_ptr = curr_ptr -> bottom;
			}
			else{
				curr_ptr = curr_ptr -> next;
			}
		}	
		while(node::compare_me(this, *(curr_ptr -> next), *mynode)){
				curr_ptr = curr_ptr -> next;
		}
		
		if( (!node::compare_me(this, *(curr_ptr -> next), *mynode) && 
			 !node::compare_me(this, *mynode, *(curr_ptr -> next))) ){
			Iterator tempI(this);
			tempI.curr = curr_ptr->next;
			return tempI;
		}
		else{
			return end();
		}
		
	}
	
	
	void remove(const_reference data){
		node *mynode = new node(alloc.allocate(1));
		*(mynode -> data) = data;
		
		node *curr_ptr = head;
		
		// Finding pos to insert
		while( curr_ptr -> bottom != nullptr){
			if( !( node::compare_me(this, *(curr_ptr -> next), *mynode) ) ){
				curr_ptr = curr_ptr -> bottom;
			}
			else{
				curr_ptr = curr_ptr -> next;
			}
		}	
		while(node::compare_me(this, *(curr_ptr -> next), *mynode)){
				curr_ptr = curr_ptr -> next;
		}
		
		if( (!node::compare_me(this, *(curr_ptr -> next), *mynode) && 
			!node::compare_me(this, *mynode, *(curr_ptr -> next))) ){
			curr_ptr = curr_ptr->next;
			alloc.deallocate(curr_ptr->data, 1);
			while(curr_ptr){
				curr_ptr->prev->next = curr_ptr->next;
				curr_ptr->next->prev = curr_ptr->prev;
				node *temp = curr_ptr;
				curr_ptr = curr_ptr->top;
				delete temp;
			}
		}
	
	}
	
	
	allocator_type get_allocator() {
		return alloc;
	}

	//capacity
	bool empty() {
		return n == 0;
	}

	size_type size() {
		return n;
	}

	size_type max_size() {
		return std::numeric_limits<size_type>::max();
	}
};

#endif
