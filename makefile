sniffer: word_list.c url_list.c parser.c ready_queue.c sniffer.c
	gcc word_list.c url_list.c parser.c ready_queue.c sniffer.c -o sniffer
	
clean:
	rm -r ../output_files/
	rm -r ../pipes/
	rm sniffer
