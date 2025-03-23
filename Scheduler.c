#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//process control block (PCB)
// in case of context switching it stores the information about the process
struct pcb 
{
	unsigned int pid;
	char pname[20];
	unsigned int ptimeleft;
	unsigned int ptimearrival;
    int check;
};

typedef struct pcb pcb;

//queue node
struct dlq_node 
{
	struct dlq_node *pfwd;
	struct dlq_node *pbck;
	struct pcb *data;
}; 

typedef struct dlq_node dlq_node;

//queue
struct dlq 
{
	struct dlq_node *head;
	struct dlq_node *tail;
};

typedef struct dlq dlq;
 
//function to add a pcb to a new queue node
dlq_node* get_new_node(pcb *ndata) 
{
	if (!ndata)
		return NULL;

	dlq_node *new = malloc(sizeof(dlq_node));
	if(!new)
    {
		fprintf(stderr, "Error: allocating memory\n");exit(1);
	}

	new->pfwd = new->pbck = NULL;
	new->data = ndata;
	return new;
}

//function to add a node to the tail of queue
void add_to_tail (dlq *q, dlq_node *new)
{
	if (!new)
		return;

	if (q->head==NULL)
    {
		if(q->tail!=NULL)
        {
			fprintf(stderr, "DLList inconsitent.\n"); exit(1);
		}
		q->head = new;
		q->tail = q->head;
	}
	else 
    {		
		new->pfwd = q->tail;
		new->pbck = NULL;
		new->pfwd->pbck = new;
		q->tail = new;
	}
}

//function to remove a node from the head of queue
dlq_node* remove_from_head(dlq * const q){
	if (q->head==NULL){ //empty
		if(q->tail!=NULL){fprintf(stderr, "DLList inconsitent.\n"); exit(1);}
		return NULL;
	}
	else if (q->head == q->tail) { //one element
		if (q->head->pbck!=NULL || q->tail->pfwd!=NULL) {
			fprintf(stderr, "DLList inconsitent.\n"); exit(1);
		}

		dlq_node *p = q->head;
		q->head = NULL;
		q->tail = NULL;
	
		p->pfwd = p->pbck = NULL;
		return p;
	}
	else { // normal
		dlq_node *p = q->head;
		q->head = q->head->pbck;
		q->head->pfwd = NULL;
	
		p->pfwd = p->pbck = NULL;
		return p;
	}
}

//function to print our queue
void print_q (const dlq *q) 
{
	dlq_node *n = q->head;
	if (n == NULL)
		return;

	while (n) 
    {
		printf("%s(%d),", n->data->pname, n->data->ptimeleft);
		n = n->pbck;
	}
}

//function to check if the queue is empty
int is_empty (const dlq *q)
{
	if (q->head == NULL && q->tail==NULL)
		return 1;
	else if (q->head != NULL && q->tail != NULL)
		return 0;
	else 
    {
		fprintf(stderr, "Error: DLL queue is inconsistent."); exit(1);
	}
}

//function to sort the queue on completion time
void sort_by_timetocompletion(const dlq *q) 
{ 
	// bubble sort
	dlq_node *start = q->tail;
	dlq_node *end = q->head;
	
	while (start != end) 
    {
		dlq_node *node = start;
		dlq_node *next = node->pfwd;

		while (next != NULL) 
        {
			if (node->data->ptimeleft < next->data->ptimeleft)
            {
				// do a swap
				pcb *temp = node->data;
				node->data = next->data;
				next->data = temp;
			}
			node = next;
			next = node->pfwd;
		}
		end = end ->pbck;
	} 
}

//function to sort the queue on arrival time
void sort_by_arrival_time (const dlq *q) 
{
	// bubble sort
	dlq_node *start = q->tail;
	dlq_node *end = q->head;
	
	while (start != end) 
    {
		dlq_node *node = start;
		dlq_node *next = node->pfwd;

		while (next != NULL) 
        {
			if (node->data->ptimearrival < next->data->ptimearrival)
            {
				// do a swap
				pcb *temp = node->data;
				node->data = next->data;
				next->data = temp;
			}
			node = next;
			next = node->pfwd;
		}
		end = end->pbck;
	}
}

//function to tokenize the one row of data
pcb* tokenize_pdata (char *buf) 
{
	pcb* p = (pcb*) malloc(sizeof(pcb));
	if(!p)
    { 
        fprintf(stderr, "Error: allocating memory.\n"); exit(1);
    }

	char *token = strtok(buf, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token pname\n"); exit(1);
    }  
	strcpy(p->pname, token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token pid\n"); exit(1);
    }  
	p->pid = atoi(token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token duration\n"); exit(1);
    } 
	 
	p->ptimeleft= atoi(token);

	token = strtok(NULL, ":\n");
	if(!token)
    { 
        fprintf(stderr, "Error: Expecting token arrival time\n"); exit(1);
    }  
	p->ptimearrival = atoi(token);

	token = strtok(NULL, ":\n");
	if(token)
    { 
        fprintf(stderr, "Error: Oh, what've you got at the end of the line?\n");exit(1);
    } 

	return p;
}



void sched_FIFO(dlq *const p_fq, int *p_time, int N)
{
  int process_time = 1;
  int tt_cum = 0;
  int rt_cum = 0;
  double throughput = 0.0;
  double tt_avg = 0.0;
  double rt_avg = 0.0;
  *p_time = 1;
  dlq ready_queue;
  ready_queue.head = NULL;
  ready_queue.tail = NULL;

  int running_process_time = 0;
  dlq_node *curr_process = NULL;

  while (!is_empty(p_fq) || !is_empty(&ready_queue) || curr_process) {
    // Check if any process arrives at the current time and move it to the ready
    // queue
    if (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time - 1) {
      dlq_node *temp = remove_from_head(p_fq);
      add_to_tail(&ready_queue, temp);
    }

    if (curr_process) {
      pcb *p = curr_process->data;

      // Simulate execution of the process
      printf("%d:%s:", *p_time, p->pname);

      // Print the contents of the ready queue
      if (is_empty(&ready_queue)) {
        printf("empty");
      } else {
        print_q(&ready_queue);
      }

      printf(":\n");

      p->ptimeleft--;

      if (p->ptimeleft > 0) {
        // Process still needs more time, continue running it
        running_process_time = p->ptimeleft;
      } else {
        // incrementing the cum turnaround time to add this process
        tt_cum += (*p_time) - p->ptimearrival;
        // Free the memory allocated for the completed process
        free(curr_process);
        curr_process = NULL;
        running_process_time = 0;
      }
    } else if (!is_empty(&ready_queue)) {
      curr_process = remove_from_head(&ready_queue);
      pcb *p = curr_process->data;
      // incrementing the cum response time to add this process
      rt_cum += (*p_time) - 1 - p->ptimearrival;
      printf("%d:%s:", *p_time, p->pname);

      // Print the contents of the ready queue
      if (is_empty(&ready_queue)) {
        printf("empty");
      } else {
        print_q(&ready_queue);
      }

      printf(":\n");

      p->ptimeleft--;
      running_process_time = p->ptimeleft;

    } else {
      // No process is ready to run, so print "idle"
      printf("%d:idle:empty:\n", *p_time);
      process_time++;
    }

    // Increment system time
    (*p_time)++;
    }
    process_time = (*p_time) - process_time; // +1 to accomodate the process coming milliseconf bef it is actually executed
    printf("time: %d\n", process_time);
    printf("No of processes: %d\n", N);
    printf("Cumulative Turnaround: %d\n", tt_cum);

    throughput = (double)N / process_time;
    tt_avg = (double)tt_cum / N;
    rt_avg = (double)rt_cum / N;

    printf("Throughput: %.3f\n", throughput);
    printf("Turnaround Time: %.3f\n", tt_avg);
    printf("Response Time: %.3f\n", rt_avg);
    printf("\n");
}

//implement the SJF scheduling code
void sched_SJF(dlq *const p_fq, int *p_time, int N)
{  
    int process_time = 1;
    int tt_cum = 0;
    int rt_cum = 0;
    double throughput = 0.0;
    double tt_avg = 0.0;
    double rt_avg = 0.0;
    *p_time = 1;
    dlq ready_queue;
    ready_queue.head = NULL;
    ready_queue.tail = NULL;

    int running_process_time = 0;
    dlq_node *curr_process = NULL;

    while (!is_empty(p_fq) || !is_empty(&ready_queue) || curr_process) {
        // Check if any process arrives at the current time and move it to the ready queue
        if (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time - 1) {
            dlq_node *temp = remove_from_head(p_fq);
            add_to_tail(&ready_queue, temp);
            sort_by_timetocompletion(&ready_queue);

        }

        if (curr_process) {
            pcb *p = curr_process->data;

            // Simulate execution of the process
            printf("%d:%s:", *p_time, p->pname);

            // Print the contents of the ready queue
            if (is_empty(&ready_queue)) {
                printf("empty");
            } else {
                print_q(&ready_queue);
            }

            printf(":\n");

            p->ptimeleft--;

            if (p->ptimeleft > 0) {
                // Process still needs more time, continue running it
                running_process_time = p->ptimeleft;
            } else {
                // incrementing the cum turnaround time to add this process
                tt_cum += (*p_time) - p->ptimearrival;
                // Free the memory allocated for the completed process
                free(curr_process);
                curr_process = NULL;
                running_process_time = 0;
            }
        } 
        else if (!is_empty(&ready_queue)) {
            sort_by_timetocompletion(&ready_queue);
            curr_process = remove_from_head(&ready_queue);
            pcb *p = curr_process->data;
            // incrementing the cum response time to add this process
            rt_cum += (*p_time) - 1 - p->ptimearrival;
            printf("%d:%s:", *p_time, p->pname);

            // Print the contents of the ready queue
            if (is_empty(&ready_queue)){
                printf("empty");
            } 
            else{
                print_q(&ready_queue);
            }

            printf(":\n");

            p->ptimeleft--;
            running_process_time = p->ptimeleft;
            
        } 
        else {
            // No process is ready to run, so print "idle"
            printf("%d:idle:empty:\n", *p_time);
            process_time += 1;
        }

        // Increment system time
        *p_time += 1;
    }
    process_time = (*p_time) - process_time;
    printf("time: %d\n", process_time);
    printf("No of processes: %d\n", N);
    // printf("Cum response: %d\n", rt_cum);

    throughput = (double)N / process_time;
    tt_avg = (double)tt_cum / N;
    rt_avg = (double)rt_cum / N;

    printf("Throughput: %.3f\n", throughput);
    printf("Turnaround Time: %.3f\n", tt_avg);
    printf("Response Time: %.3f\n", rt_avg);
    printf("\n");
}

//implement the STCF scheduling code
void sched_STCF(dlq *const p_fq, int *p_time, int N)
{
    int process_time = 1;
    int tt_cum = 0;
    int rt_cum = 0;
    double throughput = 0.0;
    double tt_avg = 0.0;
    double rt_avg = 0.0;
    *p_time = 1;
    dlq ready_queue;
    ready_queue.head = NULL;
    ready_queue.tail = NULL;

    int running_process_time = 0;
    dlq_node *curr_process = NULL;

    while (!is_empty(p_fq) || !is_empty(&ready_queue) || curr_process)
    {
        // If any process arrives at the current time, move it to the ready queue
        if (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time - 1)
        {
            dlq_node *temp = remove_from_head(p_fq);
            // Check if the current process has a shorter time to completion. 
            // If yes make it current instead
            if (curr_process && temp && temp->data->ptimeleft < curr_process->data->ptimeleft)
            {
                add_to_tail(&ready_queue, curr_process);
                // free(curr_process);
                // curr_process = NULL;
                curr_process = temp;
                running_process_time = temp->data->ptimeleft;
            }
               
            else {
                add_to_tail(&ready_queue, temp);
            }
            sort_by_timetocompletion(&ready_queue);
        }

        if (curr_process){
            pcb *p = curr_process->data;

            // Simulate execution of the process
            printf("%d:%s:", *p_time, p->pname);

            // Print the contents of the ready queue
            if (is_empty(&ready_queue)){
                printf("empty");
            }
            else{
                print_q(&ready_queue);
            }

            printf(":\n");

            p->ptimeleft--;

            if (p->ptimeleft > 0){
                // Process still needs more time, continue running it
                running_process_time = p->ptimeleft;
            }
            else{
                // Free the memory allocated for the completed process
                // incrementing the cum turnaround time to add this process
                tt_cum += (*p_time) - p->ptimearrival;
                free(curr_process);
                curr_process = NULL;
                running_process_time = 0;
            }
        }
        else if (!is_empty(&ready_queue)){
            // sort_by_timetocompletion(&ready_queue); // Sort by time to completion
            curr_process = remove_from_head(&ready_queue);
            pcb *p = curr_process->data;
            // incrementing the cum response time to add this process
            if(p->check != 1){
                rt_cum += (*p_time) - 1 - p->ptimearrival;
                p->check = 1;

            }
            printf("%d:%s:", *p_time, p->pname);

            // Print the contents of the ready queue
            if (is_empty(&ready_queue)){
                printf("empty");
            }
            else{
                print_q(&ready_queue);
            }

            printf(":\n");

            p->ptimeleft--;
            running_process_time = p->ptimeleft;
        }
        else{
            // No process is ready to run, so print "idle"
            printf("%d:idle:empty:\n", *p_time);
            process_time++;
        }

        // Increment system time
        (*p_time)++;
    }
    process_time = (*p_time) - process_time;
    printf("time: %d\n", process_time);
    printf("No of processes: %d\n", N);

    throughput = (double)N / process_time;
    tt_avg = (double)tt_cum / N;
    rt_avg = (double)rt_cum / N;

    printf("Throughput: %.3f\n", throughput);
    printf("Turnaround Time: %.3f\n", tt_avg);
    printf("Response Time: %.3f\n", rt_avg);
    printf("\n");
}


// Implement the RR scheduling code
void sched_RR(dlq *const p_fq, int *p_time, int N)
{
    int process_time = 1;
    int tt_cum = 0;
    int rt_cum = 0;
    double throughput = 0.0;
    double tt_avg = 0.0;
    double rt_avg = 0.0;
    *p_time = 1;
    dlq ready_queue;
    ready_queue.head = NULL;
    ready_queue.tail = NULL;

    dlq_node *curr_process = NULL;

    while (!is_empty(p_fq) || !is_empty(&ready_queue) || curr_process){
        // If any process arrives at the current time, move it to the ready queue
        if (!is_empty(p_fq) && p_fq->head->data->ptimearrival == *p_time - 1){
            dlq_node *temp = remove_from_head(p_fq);
            add_to_tail(&ready_queue, temp);
        }
        if (!is_empty(&ready_queue)){
            curr_process = remove_from_head(&ready_queue);
            pcb *p = curr_process->data;
            // incrementing the cum response time to add this process
            if(p->check != 1){
                rt_cum += (*p_time) - 1 - p->ptimearrival;
                p->check = 1;
            }
            printf("%d:%s:", *p_time, p->pname);

            // Print the contents of the ready queue
            if (is_empty(&ready_queue)){
                printf("empty");
            }
            else{
                print_q(&ready_queue);
            }

            printf(":\n");

            p->ptimeleft--;

            if (p->ptimeleft > 0){
                // Process still needs more time, put it back in the ready queue
                add_to_tail(&ready_queue, curr_process);
            }
            else if(p->ptimeleft == 0){
                // incrementing the cum turnaround time to add this process
                tt_cum += (*p_time) - p->ptimearrival;
            }
            curr_process = NULL;
        }
        else{
            // No process is ready to run, so print "idle" and indicate that the queue is empty
            printf("%d:idle:empty:\n", *p_time);
            process_time++;
        }

        // Increment system time
        (*p_time)++;

    }
    process_time = (*p_time) - process_time;
    printf("time: %d\n", process_time);
    printf("No of processes: %d\n", N);

    throughput = (double)N / process_time;
    tt_avg = (double)tt_cum / N;
    rt_avg = (double)rt_cum / N;

    printf("Throughput: %.3f\n", throughput);
    printf("Turnaround Time: %.3f\n", tt_avg);
    printf("Response Time: %.3f\n", rt_avg);
    printf("\n");
}

int main()
{
    /* Enter your code here. Read input from STDIN. Print output to STDOUT */
    int N = 0;
    char tech[20]={'\0'};
    char buffer[100]={'\0'};
    scanf("%d", &N);
    // printf("%d\n", N);
    scanf("%s", tech);
    // printf("%s\n", tech);
     
    dlq queue;
    queue.head = NULL;
    queue.tail = NULL;
    for(int i=0; i<N; ++i)
    {   
        scanf("%s\n", buffer); 
        // printf("%s\n", buffer); 
        pcb *p = tokenize_pdata(buffer);
        add_to_tail (&queue, get_new_node(p) );  
    }
    // print_q(&queue);
    unsigned int system_time = 0;
    sort_by_arrival_time (&queue);
    // print_q (&queue);
    
    // run scheduler
    if(!strncmp(tech,"FIFO",4))
        sched_FIFO(&queue, &system_time, N);
    else if(!strncmp(tech,"SJF",3))
        sched_SJF(&queue, &system_time, N);
    else if(!strncmp(tech,"STCF",4))
        sched_STCF(&queue, &system_time, N);
    else if(!strncmp(tech,"RR",2))
        sched_RR(&queue, &system_time, N);
    else
        fprintf(stderr, "Error: unknown POLICY\n");
    return 0;
}