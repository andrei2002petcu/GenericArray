#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "structs.h"

void print(void *arr, int len) {

	int cnt = 0; //variabila ce numara fiecare byte parcurs de *ptr
	unsigned char tip;
	void *ptr = arr; //pointer de parcurgere al vectorului

	//se parcurge vectorul cat timp raman bytes neparcursi
	while (cnt < len) {
		//extragerea si afisarea tipului + deplasarea pointer-ului
		tip = *(unsigned char *)ptr;
		printf ("Tipul %c\n", tip);
		ptr += 8;
		cnt += 8;

		//se parcurge primul nume (pana cand intalneste '\0') si afiseaza fiecare caracter
		while (*(char *)ptr != '\0') {
			printf ("%c", *(char *)ptr);
			ptr += 1;
			cnt += 1;
		}
		printf (" pentru ");
		
		//sarim peste caracterul '\0'
		ptr += 1;
		cnt += 1;

		//in functie de tipul datelor, se extrag bancnotele
		//se afiseaza al doilea nume (identic cu prima afisare de nume)
		// se afiseaza bancnotele
		if (tip == '1') {
			int8_t suma1 = *(int8_t *)ptr;
			ptr += 1;
			cnt += 1;
			int8_t suma2 = *(int8_t *)ptr;
			ptr += 1;
			cnt += 1;

			while (*(char *)ptr != '\0') {
				printf ("%c", *(char *)ptr);
				ptr += 1;
				cnt += 1;
			}

			printf ("\n");
			printf ("%"PRId8"\n", suma1);
			printf ("%"PRId8"\n", suma2);
		}
		else if (tip == '2') {
			int16_t suma1 = *(int16_t *)ptr;
			ptr += 2;
			cnt += 2;
			int32_t suma2 = *(int32_t *)ptr;
			ptr += 4;
			cnt += 4;

			while (*(char *)ptr != '\0') {
				printf ("%c", *(char *)ptr);
				ptr += 1;
				cnt += 1;
			}

			printf ("\n");
			printf ("%"PRId16"\n", suma1);
			printf ("%"PRId32"\n", suma2);
		}
		else if (tip == '3') {
			int32_t suma1 = *(int32_t *)ptr;
			ptr += 4;
			cnt += 4;
			int32_t suma2 = *(int32_t *)ptr;
			ptr += 4;
			cnt += 4;

			while (*(char *)ptr != '\0') {
				printf ("%c", *(char *)ptr);
				ptr += 1;
				cnt += 1;
			}

			printf ("\n");
			printf ("%"PRId32"\n", suma1);
			printf ("%"PRId32"\n", suma2);
		}

		printf ("\n");
		ptr += 1;
		cnt += 1;
	}
}

//////////////////////// TASK 1 ///////////////////////////
int add_last(void **arr, int *len, data_structure *data) {

	//se verifica daca vectorul este gol
	if (*len == 0) {
		
		//completarea lui 'len' si alocarea vectorului
		*len = data->header->len + sizeof (head);
		*arr = malloc (data->header->len + sizeof (head));
		
		//se verifica daca alocarea s-a efectuat
		if (*arr == NULL)
			return -1;

		void *ptr = *arr;
		//copiem header-ul in vector (inclusiv padding-ul)
		memcpy (ptr, data->header, sizeof (head));
		ptr += 8;
		//copiem datele in vector
		memcpy (ptr, data->data, data->header->len);
	}
	else {

		//realocarea vectorului adaugand dimensiunea noului element
		*arr = realloc (*arr, *len + data->header->len + sizeof (data->header));
		
		//pozitionam pointer-ul de parcurgere la final
		void *ptr = *arr;
		ptr += *len;
		
		//se verifica daca realocarea s-a efectuat
		if (ptr == NULL)
			return -1;

		//actualizam lungimea	
		*len += (data->header->len + sizeof (data->header));

		//copiem datele si header-ul in vector
		memcpy (ptr, data->header, sizeof (head));
		ptr += 8;
		memcpy (ptr, data->data, data->header->len);
	}
	return 0;
}

//////////////////////// TASK 2 ///////////////////////////
int add_at(void **arr, int *len, data_structure *data, int index) {

	if (index < 0)
		return -1;

	//parcurgem vectorul in aceasi maniera ca in functia de printare
	//'cnt' numara fiecare byte parcurs, index_x numara cate elemente am parcurs
	int cnt = 0, len_x, index_x = 0;
	void *ptr = *arr;
	//se parcurge pana la index-ul potrivit sau pana la finalul vectorului
	while (index_x != index && cnt < *len) {
		ptr += 4;
		len_x = *(unsigned int *)ptr;
		ptr += 4;
		ptr += len_x;
		cnt += 8 + len_x;

		index_x++;
	}

	//daca index-ul este mai mare decat numarul de elemente, se apeleaza functia add_last
	if (cnt == *len) {
		add_last (arr, len, data);
		return 0;
	}
	
	//realocam vectorul cu dimensiunea noilor date
	*arr = realloc (*arr, *len + data->header->len + sizeof (data->header));
	
	//salvam datele (de la adr unde vom adauga) intr-un vector auxiliar
	ptr = *arr + cnt;
	void *aux = malloc (*len - cnt);
	memcpy (aux, ptr, *len - cnt);
	
	//se adauga datele la vectorul initial
	memcpy (ptr, data->header, 8);
	ptr += 8;
	memcpy (ptr, data->data, data->header->len);
	ptr += data->header->len;

	//lipim vechile date (salvate in vectorul auxiliar) si actualizam lungimea
	memcpy (ptr, aux, *len - cnt);
	*len += data->header->len + 8;
	
	free (aux);
	return 0;
}

//////////////////////// TASK 3 ///////////////////////////
void find(void *data_block, int len, int index) {

	if (index < 0)
		return;

	//parcurgem vectorul in aceasi maniera ca in functia add_to
	int cnt = 0, len_x, index_x = 0;
	void *ptr = data_block;
	while (index_x != index && cnt < len) {
		index_x++;

		ptr += 4;
		len_x = *(unsigned int *)ptr;
		ptr += 4;
		ptr += len_x;
		cnt += 8 + len_x;
	}

	if (index_x < index)
		return;
	
	//extragem lungimea datelor si apelam functia de print
	ptr += 4;
	len_x = *(unsigned int *)ptr;
	print (ptr-4, len_x);
}

//////////////////////// TASK 4 ///////////////////////////
int delete_at(void **arr, int *len, int index) {

	if (index < 0)
		return -1;

	//parcurgem vectorul in aceasi maniera ca in functia add_to
	int cnt = 0, len_x, index_x = 0;
	void *ptr = *arr;
	while (index_x != index && cnt < *len) {
		index_x++;

		ptr += 4;
		len_x = *(unsigned int *)ptr;
		ptr += 4;
		ptr += len_x;
		cnt += 8 + len_x;
	}

	if (index_x < index)
		return -1;

	//se extrage lungimea datelor pt eliminare
	//eliminam prin suprascrierea elentului urmator peste cel curent
	len_x = *(unsigned int *)(ptr + 4);
	memcpy (ptr, (ptr + 8 + len_x), *len - 8 - len_x - cnt);
	
	//actualizam lungimea si realocam vectorul
	*len = *len - 8 - len_x;
	*arr = realloc (*arr, *len);

	return 0;
}

int main() {
	
	void *arr = NULL;
	int len = 0;
	char *buffer = malloc(256);

	while (1) {
		//citim comenzile 
		scanf ("%s", buffer);

		//ramura pentru insert si insert_at
		if (strstr (buffer, "insert") == buffer) {
		
			unsigned char tip;
			int suma1, suma2, index;
			char *nume1 = malloc (256);
			char *nume2 = malloc (256);

			//citim datele de la tastatura
			//in cazul insert_at, se citeste si un index
			if (strcmp (buffer, "insert_at") == 0)
				scanf (" %d %c %s %d %d %s ", &index, &tip, nume1, &suma1, &suma2, nume2);
			else
				scanf (" %c %s %d %d %s ", &tip, nume1, &suma1, &suma2, nume2);

			nume1 = realloc (nume1, strlen (nume1) + 1);
			nume2 = realloc (nume2, strlen (nume2) + 1);

			//alocarea structurii, adaugarea lungimilor numelor in 'len' si completarea tipului		
			data_structure *data_x = malloc (sizeof (data_structure));
			data_x->header = malloc (sizeof (head));
			data_x->header->len = strlen (nume1) + strlen (nume2) + 2;
			data_x->header->type = tip;
			
			//completarea in functie de tip a lui 'len' si alocarea pentru 'data'
			if (tip == '1')
				data_x->header->len += 2;
			else if (tip == '2')
				data_x->header->len += 6;
			else
				data_x->header->len += 8;
			data_x->data = malloc (data_x->header->len);
			
			//adaugam primul nume in 'data'
			void *ptr = data_x->data; //pointer ce va parcurge 'data' pentru adaugarile datelor
			memcpy (ptr, nume1, strlen (nume1) + 1);
			ptr += strlen (nume1) + 1;

			//adaugam bancnotele in 'data' (tinem cont de tip) si deplasam 'ptr'
			if (tip == '1') {
				int8_t int_x = (int8_t)suma1;
				memcpy (ptr, &int_x, 1);
				ptr += 1;
				int_x = (int8_t)suma2;
				memcpy (ptr, &int_x, 1);
				ptr += 1;
			}
			else if (tip == '2') {
				int16_t int_x = (int16_t)suma1;
				memcpy (ptr, &int_x, 2);
				ptr += 2;
				int32_t int_y = (int32_t)suma2;
				memcpy (ptr, &int_y, 4);
				ptr += 4;
			}
			else if (tip == '3') {
				int32_t int_x = (int32_t)suma1;
				memcpy (ptr, &int_x, 4);
				ptr += 4;
				int_x = (int32_t)suma2;
				memcpy (ptr, &int_x, 4);
				ptr += 4;
			}
			//adaugam al doilea nume in 'data'
			memcpy (ptr, nume2, strlen (nume2) + 1);

			//se apeleaza functiile corespunzatoare comenzii citite initial
			if (strcmp (buffer, "insert_at") == 0)
				add_at (&arr, &len, data_x, index);
			else
				add_last (&arr, &len, data_x);
			
			//dezalocam memoria
			free (nume1);
			free (nume2);
			free (data_x->header);
			free (data_x->data);
			free (data_x);
		}

		//ramura pentru comanda find -> citeste un index apoi apeleaza functia necesara
		if (strcmp (buffer, "find") == 0) {
			int index;
			scanf ("%d", &index);
			find (arr, len, index);
		}

		//ramura pentru comanda delete -> citeste un index apoi apeleaza functia necesara
		if (strcmp (buffer, "delete_at") == 0) {
			int index;
			scanf ("%d", &index);
			delete_at (&arr, &len, index);
		}

		//ramura pentru comanda print -> apeleaza functia de printare
		if (strcmp (buffer, "print") == 0)
			print (arr, len);
		
		//ramura pentru comanda exit -> elibereaza memoria si iese din while
		if (strcmp (buffer, "exit") == 0) {
			free (arr);
			free (buffer);
			break;
		}
	}

	return 0;
}