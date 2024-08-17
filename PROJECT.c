#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAX_FILENAME_LENGTH 100
#define MAX_CONTENT_LENGTH 1024
#define MAX_DIRS 100
#define MAX_FILES 100

typedef struct {
    char name[MAX_FILENAME_LENGTH];
    char *content;
    int size;
} File;

typedef struct Directory {
    char name[MAX_FILENAME_LENGTH];
    int num_files;
    File *files[MAX_FILES];
    int num_subdirs;
    struct Directory *subdirs[MAX_DIRS];
} Directory;

typedef struct {
    Directory root;
    Directory *cwd; 
} FileSystem;

void init_filesystem(FileSystem *fs);
void create_file(FileSystem *fs);
void list_files(FileSystem *fs);
void create_directory(FileSystem *fs);
void change_directory(FileSystem *fs);
void view(FileSystem *fs);
void view_directories(Directory *dir);
void delete_directory(FileSystem *fs);
void file_system_manager();
void main_menu();


typedef struct Process {
    int id;
    int priority;
    struct Process *next;
} Process;

typedef struct PriorityQueue {
    Process *front;
} PriorityQueue;

void enqueue(PriorityQueue *pq, int id, int priority);
void dequeue(PriorityQueue *pq);
void killProcess(PriorityQueue *pq, int id);
void displayQueue(PriorityQueue *pq);
void executeProcesses(PriorityQueue *pq);


int main();


void init_filesystem(FileSystem *fs) {
    strcpy(fs->root.name, "/");
    fs->root.num_files = 0;
    fs->root.num_subdirs = 0;
    fs->cwd = &fs->root; 
}

void create_file(FileSystem *fs) {
    char name[MAX_FILENAME_LENGTH];
    char content[MAX_CONTENT_LENGTH];

    printf("Enter file name: ");
    fgets(name, MAX_FILENAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = '\0'; 

    
    for (int i = 0; i < fs->cwd->num_files; i++) {
        if (strcmp(fs->cwd->files[i]->name, name) == 0) {
            printf("File '%s' already exists.\n", name);
            return;
        }
    }

    printf("Enter file content: ");
    fgets(content, MAX_CONTENT_LENGTH, stdin);
    content[strcspn(content, "\n")] = '\0'; 

    if (fs->cwd->num_files < MAX_FILES) {
        File *new_file = malloc(sizeof(File));
        if (new_file == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        new_file->content = strdup(content);
        if (new_file->content == NULL) {
            printf("Memory allocation failed.\n");
            free(new_file);
            return;
        }
        strcpy(new_file->name, name);
        new_file->size = strlen(content);
        fs->cwd->files[fs->cwd->num_files] = new_file;
        fs->cwd->num_files++;
        printf("File '%s' created.\n", name);
    } else {
        printf("Cannot create file. Directory is full.\n");
    }
}

void list_files(FileSystem *fs) {
    Directory *dir = fs->cwd;
    printf("Files in directory '%s':\n", dir->name);
    for (int i = 0; i < dir->num_files; i++) {
        printf("%s\n", dir->files[i]->name);
    }
}

void create_directory(FileSystem *fs) {
    char dirname[MAX_FILENAME_LENGTH];
    printf("Enter directory name: ");
    fgets(dirname, MAX_FILENAME_LENGTH, stdin);
    dirname[strcspn(dirname, "\n")] = '\0'; 

    Directory *dir = fs->cwd;
    if (dir->num_subdirs < MAX_DIRS) {
        for (int i = 0; i < dir->num_subdirs; i++) {
            if (strcmp(dir->subdirs[i]->name, dirname) == 0) {
                printf("Directory '%s' already exists.\n", dirname);
                return;
            }
        }

        Directory *new_dir = malloc(sizeof(Directory));
        if (new_dir == NULL) {
            printf("Memory allocation failed.\n");
            return;
        }
        strcpy(new_dir->name, dirname);
        new_dir->num_files = 0;
        new_dir->num_subdirs = 0;
        dir->subdirs[dir->num_subdirs] = new_dir;
        dir->num_subdirs++;
        printf("Directory '%s' created.\n", dirname);
    } else {
        printf("Cannot create directory. Maximum subdirectories reached.\n");
    }
}

void change_directory(FileSystem *fs) {
    char dirname[MAX_FILENAME_LENGTH];
    printf("Enter directory name (or '..' for parent directory): ");
    fgets(dirname, MAX_FILENAME_LENGTH, stdin);
    dirname[strcspn(dirname, "\n")] = '\0'; 

    if (strcmp(dirname, "..") == 0) {
        if (fs->cwd != &fs->root) {
            fs->cwd = &fs->root;
            printf("Changed directory to parent directory.\n");
        } else {
            printf("Already at root directory.\n");
        }
        return;
    }

    Directory *dir = fs->cwd;
    for (int i = 0; i < dir->num_subdirs; i++) {
        if (strcmp(dir->subdirs[i]->name, dirname) == 0) {
            fs->cwd = dir->subdirs[i];
            printf("Changed directory to '%s'.\n", dirname);
            return;
        }
    }
    printf("Directory '%s' not found.\n", dirname);
}

void view(FileSystem *fs) {
    char filename[MAX_FILENAME_LENGTH];
    printf("Enter File Name: ");
    fgets(filename, MAX_FILENAME_LENGTH, stdin);
    filename[strcspn(filename, "\n")] = '\0'; 

    Directory *dir = fs->cwd;
    for (int i = 0; i < dir->num_files; i++) {
        if (strcmp(dir->files[i]->name, filename) == 0) {
            printf("Content of '%s':\n%s\n", filename, dir->files[i]->content);
            return;
        }
    }
    printf("File '%s' not found.\n", filename);
}

void view_directories(Directory *dir) {
    printf("Subdirectories in directory '%s':\n", dir->name);
    for (int i = 0; i < dir->num_subdirs; i++) {
        printf("%s\n", dir->subdirs[i]->name);
    }
}

void delete_directory(FileSystem *fs) {
    char dirname[MAX_FILENAME_LENGTH];
    printf("Enter directory name to delete: ");
    fgets(dirname, MAX_FILENAME_LENGTH, stdin);
    dirname[strcspn(dirname, "\n")] = '\0'; 

    Directory *dir = fs->cwd;
    for (int i = 0; i < dir->num_subdirs; i++) {
        if (strcmp(dir->subdirs[i]->name, dirname) == 0) {
            free(dir->subdirs[i]);
            for (int j = i; j < dir->num_subdirs - 1; j++) {
                dir->subdirs[j] = dir->subdirs[j + 1];
            }
            dir->num_subdirs--;
            printf("Directory '%s' deleted.\n", dirname);
            return;
        }
    }
    printf("Directory '%s' not found.\n", dirname);
}

void file_system_manager() {
    FileSystem fs;
    init_filesystem(&fs);

    int choice;
    do {
        printf("\nFile Manager MENU:\n");
        printf("1. Create File\n");
        printf("2. List Files\n");
        printf("3. Create Directory\n");
        printf("4. Change Directory\n");
        printf("5. View File Contents\n");
        printf("6. View Directories\n");
        printf("7. Delete Directory\n");
        printf("8. Back\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch (choice) {
            case 1:
                create_file(&fs);
                break;
            case 2:
                list_files(&fs);
                break;
            case 3:
                create_directory(&fs);
                break;
            case 4:
                change_directory(&fs);
                break;
            case 5:
                view(&fs);
                break;
            case 6:
                view_directories(fs.cwd);
                break;
            case 7:
                delete_directory(&fs);
                break;
            case 8:
                main_menu();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);
}

PriorityQueue* initPriorityQueue() {
    PriorityQueue *pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (pq == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    pq->front = NULL;
    return pq;
}


void enqueue(PriorityQueue *pq, int id, int priority) {

    Process *newProcess = (Process*)malloc(sizeof(Process));
    if (newProcess == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newProcess->id = id;
    newProcess->priority = priority;
    newProcess->next = NULL;


    if (pq->front == NULL || priority < pq->front->priority) {
        newProcess->next = pq->front;
        pq->front = newProcess;
    } else {
        Process *current = pq->front;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newProcess->next = current->next;
        current->next = newProcess;
    }
}

void dequeue(PriorityQueue *pq) {
    if (pq->front == NULL) {
        printf("Queue is empty\n");
    } else {
        Process *temp = pq->front;
        pq->front = pq->front->next;
        free(temp);
    }
}

void killProcess(PriorityQueue *pq, int id) {
    Process *current = pq->front;
    Process *prev = NULL;
    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }
    if (current == NULL) {
        printf("Process %d not found\n", id);
    } else {
        if (prev == NULL) {
            pq->front = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
        printf("Process %d killed\n", id);2
    }
}

void displayQueue(PriorityQueue *pq) {
    printf("Initialized processes in priority queue: \n");
    Process *current = pq->front;
    while (current != NULL) {
        printf("ID: %d, Priority: %d\n", current->id, current->priority);
        current = current->next;
    }
}

void executeProcesses(PriorityQueue *pq) {
    Process *current = pq->front;
    while (current != NULL) {
        printf("Process ID: %d, Priority: %d started execution\n", current->id, current->priority);
        fflush(stdout); 
        sleep(10); 
        printf("\nProcess ID: %d, Priority: %d completed execution\n", current->id, current->priority);
        fflush(stdout); 
        current = current->next; 
    }
    dequeue(pq);
}

void process_manager() {
    srand(time(NULL)); 

    char choice;
    PriorityQueue *pq = initPriorityQueue();

    do {
        printf("\nProcess Manager MENU:\n");
        printf("1. Initialize a process\n");
        printf("2. Kill a process\n");
        printf("3. Execute initialized processes\n");
        printf("4. Display initialized processes\n");
        printf("5. Display uninitialized processes\n");
        printf("6. Back\n");

        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1': {
                int id, priority;
                printf("Enter the process ID to initialize: ");
                scanf("%d", &id);
                printf("Enter the priority number for the process: ");
                scanf("%d", &priority);
                enqueue(pq, id, priority);
                printf("Process ID: %d initialized with priority %d\n", id, priority);
                break;
            }
            case '2': {
                int id;
                printf("Enter the process ID to kill: ");
                scanf("%d", &id);
                killProcess(pq, id);
                break;
            }
            case '3': {
                executeProcesses(pq);
                break;
            }
            case '4': {
                displayQueue(pq);
                break;
            }
            case '5': {
                printf("Uninitialized processes: ");
                for (int id = 0; id < 1000; id++) {
                    Process *current = pq->front;
                    int found = 0;
                    while (current != NULL) {
                        if (current->id == id) {
                            found = 1;
                            break;
                        }
                        current = current->next;
                    }
                    if (!found) {
                        printf("ID: %d\n", id);
                    }
                }
                break;
            }
            case '6': {
                printf("Exiting program...\n");
                main_menu();
                break;
            }
            default: {
                printf("Invalid choice. Please enter a valid option.\n");
            }
        }
    } while (choice != '7');

    Process *current = pq->front;
    while (current != NULL) {
        Process *temp = current;
        current = current->next;
        free(temp);
    }
    free(pq);
}


int main() {
    main_menu();
    return 0;
}

void main_menu() {
    char choice;
    do {
        printf("\nMain MENU:\n");
        printf("1. File System Operations\n");
        printf("2. Process Manager\n");
        printf("3. Exit\n");

        printf("Enter your choice: ");
        scanf(" %c", &choice);

        switch (choice) {
            case '1': {
                printf("Entering File System Manager...\n");
                file_system_manager();
                break;
            }
            case '2': {
                printf("Entering Process Manager...\n");
                process_manager();
                break;
            }
            case '3': {
                printf("Exiting program...\n");
                choice = 3;
                exit(0);
                break;
            }
            default: {
                printf("Invalid choice. Please enter a valid option.\n");
            }
        }
    } while (choice != '3');
}
