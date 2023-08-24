#include<stdio.h>
#include<stdlib.h>

int main() {
    int n, i, head, total = 0;
    
    printf("Enter the number of requests: ");
    scanf("%d", &n);
    
    int requests[n];
    
    printf("Enter the request queue:\n");
    for(i = 0; i < n; i++) {
        scanf("%d", &requests[i]);
    }
    
    printf("Enter the initial head position: ");
    scanf("%d", &head);
    
    printf("Head Movement Order:\n");
    for(i = 0; i < n; i++) {
        printf("%d -> ", head);
        total += abs(requests[i] - head);
        head = requests[i];
    }
    printf("%d -> ", head);
    printf("End\n");
    
    printf("Total head movements: %d\n", total);
    
    return 0;
}

