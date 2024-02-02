#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 100

#define FIELD_FILE "fields.cfg"
#define MENU_FILE "menu.cfg"
#define DATA_FILE "data.dat"
#define MESSAGE_FILE "customized_user_message.cfg"

FILE *fp_fields;
FILE *fp_menu;
FILE *fp_data;
FILE *fp_message;


char **fields_names;
int fields_count;
int field_length;
char *field_data;
char *field_value;
int record_position;
char **message;
int messages_count;
char *functionality;
int is_record_found;

int get_fields_count()
{
	fields_count = 0;
	char field_line[LENGTH];
	// fp_fields = fopen(FIELD_FILE, "r");
	while(fgets(field_line, LENGTH, fp_fields))
	{
		fields_count++;
	}
	return fields_count;
}

char **get_fields_array()
{
	int fields_counter = 0;
	fields_count = get_fields_count();
	rewind(fp_fields);
	fields_names = (char**)malloc(sizeof(char*) * fields_count);
	for (fields_counter = 0; fields_counter < fields_count; fields_counter++)
	{
		fields_names[fields_counter] = (char*)malloc(sizeof(char) * LENGTH);
		fgets(fields_names[fields_counter], LENGTH, fp_fields);
		fields_names[fields_counter][strlen(fields_names[fields_counter]) - 1] = '\0';
		// printf("%s", fields_names[fields_counter]);
	} 
	return fields_names;
	fclose(fp_fields);
}

int get_field_length()
{
	field_length = 0;
	FILE *fp_field_length = fopen("field_length.cfg", "r");
	fscanf(fp_field_length, "%d", &field_length);
	fclose(fp_field_length);
	// printf("%d", field_length);
	return field_length;
}

char* get_fields_data(char field_name[])
{
	field_data = malloc(sizeof(char) * field_length);
	printf("Enter %s: ", field_name);
	scanf(" %[^\n]s", field_data);
	// printf("%s", field_data);
	return field_data;
}

void print_record_create_status(int fields_counter)
{

	if (fields_counter == fields_count + 1)
	{
		printf("%s %s.\n", message[7], message[1] );
	}
	else
	{
		printf("%s %s.\n", message[7], message[2]);
	}	
}

int add_record()
{
	int fields_counter = 0;
	field_value = (char*)malloc(sizeof(char) * field_length);
	int count_of_fields_added = 0 ;
	for (fields_counter = 0; fields_counter < fields_count ; fields_counter++)
	{
		field_value = get_fields_data(fields_names[fields_counter]);
		fp_data = fopen(DATA_FILE, "a");
		count_of_fields_added = count_of_fields_added + fwrite(field_value, field_length, 1, fp_data);
		// printf("%d", ftell(fp_data));
		fclose(fp_data);
	}
	fp_data = fopen(DATA_FILE, "a");
	strcpy(field_value, "A");
	count_of_fields_added = count_of_fields_added + fwrite(field_value, field_length, 1, fp_data);
	fclose(fp_data);
	// return count_of_fields_added;
	print_record_create_status(count_of_fields_added);
}


void show_records()
{
	fp_data = fopen(DATA_FILE, "r");
	char status[field_length];
	char record[fields_count + 1][field_length];
	int fields_counter = 0;
	while(fread(record, field_length, fields_count, fp_data))
	{
		fread(status, field_length, 1, fp_data);
		if(!strcmp(status, "A"))
		{
			for (fields_counter = 0; fields_counter < fields_count; fields_counter++)
			{
				printf("%s: %s\n", fields_names[fields_counter], record[fields_counter]);
			}
			printf("\n");
		}
	}
	fclose(fp_data);
}


char *get_record_id()
{
	field_value = malloc(sizeof(char) * field_length);
	printf("Enter %s: ", fields_names[0]);
	scanf("%s", field_value);
	return field_value;

}

int get_record_position(char record_id[])
{
	char record[fields_count + 1][field_length];
	record_position = -1;
	fp_data = fopen(DATA_FILE, "r");
	while(fread(record, field_length, fields_count + 1, fp_data))
	{
		if((!strcmp(record[fields_count], "A")) && (!strcmp(record[0], record_id)))
		{
			record_position = ftell(fp_data) - (field_length * (fields_count + 1));
			is_record_found = 1;
			break;
		}
	}
	fclose(fp_data);
	return record_position;
}

void search_record(char record_id[])
{
	char record[fields_count + 1][field_length];
	char field_value[field_length];
	int fields_counter = 0;
	int is_record_found = 0;
	int record_position = get_record_position(record_id);
	if (record_position != -1)
	{
		fp_data = fopen(DATA_FILE, "r");
		fseek(fp_data, record_position, SEEK_SET);
		fread(record, field_length, fields_count + 1, fp_data);
		for(fields_counter = 0; fields_counter < fields_count; fields_counter++)
		{
			 printf("%s: %s\n", fields_names[fields_counter], record[fields_counter]);
		}
	}
	else
	{
		printf("%s %s %s.\n", message[0], record_id, message[3]);
	}
	fclose(fp_data);
}



char *get_new_value()
{
	char *new_value = (char*)malloc(sizeof(char) * field_length);
	printf("Enter new %s: ", fields_names[fields_count - 1]);
	scanf(" %[^\n]s", new_value);
	return new_value;
}

void print_record_update_status(char record_id[], int is_record_updated, char functionality[])
{
	if (is_record_found)
	{
		if ((is_record_updated) && (!strcmp(functionality,"update")))
		{
			printf("%s %s %s.\n", message[0], record_id, message[4]);
		}
		else if ((is_record_updated) && (!strcmp(functionality ,"delete")))
		{
			printf("%s %s %s.\n", message[0], record_id, message[6]);
		}
		else
		{
			printf("%s %s.", message[5], functionality);
		}	
	}
	else
	{
		printf("%s %s %s.\n", message[0], record_id, message[3]);
	}
}
 

void update_record_details(char record_id[])
{
	char record[fields_count + 1][field_length];
	functionality = (char*)malloc(sizeof(char) * field_length);
	strcpy(functionality, "update");
	int record_position = get_record_position(record_id);
	char *new_value;
	int is_record_updated = 0;
	if (record_position != -1)
	{
		new_value = get_new_value();
		fp_data = fopen(DATA_FILE, "r+");
		fseek(fp_data, record_position, SEEK_SET);
		fread(record, field_length, fields_count + 1, fp_data);
		strcpy(record[fields_count - 1], new_value);
		fseek(fp_data, - field_length*(fields_count+1), SEEK_CUR);
		is_record_updated = fwrite(record, field_length, fields_count + 1, fp_data);
		fclose(fp_data);
	}
	print_record_update_status(record_id, is_record_updated, functionality);

}

int get_customized_user_message_count()
{
	messages_count = 0;
	char message_line[LENGTH];
	fp_message = fopen(MESSAGE_FILE, "r");
	while(fgets(message_line, LENGTH, fp_message))
	{
		messages_count++;
	}
	// printf("%d", messages_count);
	return messages_count;
	// fclose(fp_message);
}

char **get_customized_user_message_array()
{
	int messages_counter = 0;
	messages_count = get_customized_user_message_count();
	// printf("%d", messages_count);
	rewind(fp_message);
	message = (char**)malloc(sizeof(char*) * messages_count);
	for (messages_counter = 0; messages_counter < messages_count; messages_counter++)
	{
		message[messages_counter] = (char*)malloc(sizeof(char) * LENGTH);
		fgets(message[messages_counter], LENGTH, fp_message);
		message[messages_counter][strlen(message[messages_counter]) - 1] = '\0';
		// printf("%s", message[messages_counter]);
	} 
	return message;
	fclose(fp_message);
}


void delete_record(char record_id[])
{
	char record[fields_count + 1][field_length];
	functionality = (char*)malloc(sizeof(char) * field_length);
	strcpy(functionality, "delete");
	int record_position = get_record_position(record_id);
	int is_record_updated = 0;
	if (record_position != -1)
	{
		fp_data = fopen(DATA_FILE, "r+");
		fseek(fp_data, record_position, SEEK_SET);
		fread(record, field_length, fields_count + 1, fp_data);
		strcpy(record[fields_count], "D");
		fseek(fp_data, - field_length*(fields_count+1), SEEK_CUR);
		is_record_updated = fwrite(record, field_length, fields_count + 1, fp_data);
		fclose(fp_data);
	}
	print_record_update_status(record_id, is_record_updated, functionality);
}

int get_choice()
{
	int choice;
	printf("Enter your choice: ");
	scanf("%d", &choice);
	return choice;
}

void show_menu()
{
	char menu_line[LENGTH];
	fp_menu = fopen(MENU_FILE, "r");
	while(fgets(menu_line, LENGTH, fp_menu))
	{
		printf("%s", menu_line);
	}
	printf("\n");
	int choice = get_choice();

	switch(choice)
	{
		case 1:
			printf("To add new record.\n");
			add_record();
			break;
		case 2:
			printf("To show all records.\n");
			show_records();
			break;
		case 3:
			printf("To search record.\n");
			search_record(get_record_id());
			break;
		case 4:
			printf("To update record details.\n");
			update_record_details(get_record_id());
			break;
		case 5:
			printf("To delete record.\n");
			delete_record(get_record_id());
			break;
		case 0:
			exit(0);
			fclose(fp_fields);
			break;
		default:
			printf("Invalid option.\n");
			break;

	}

	fclose(fp_menu);
}


void main()
{
	fp_fields = fopen(FIELD_FILE, "r");
	field_length = get_field_length();
	fields_names = get_fields_array();
	message = get_customized_user_message_array();
	while(1)
	{
		show_menu();
	}
}