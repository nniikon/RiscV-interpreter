static unsigned int random_seed = 12345;

unsigned int simple_rand()
{
    random_seed = random_seed * 1103515245 + 12345;
    return random_seed >> 16;
}

void swap( int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition( int arr[], int low, int high)
{
    int pivot = arr[high];
    int i = low - 1;
    
    for ( int j = low; j <= high - 1; j++ )
    {
        if ( arr[j] <= pivot )
        {
            i++;
            swap( &arr[i], &arr[j]);
        }
    }
    swap( &arr[i + 1], &arr[high]);
    return i + 1;
}

void quick_sort( int arr[], int low, int high)
{
    if ( low < high )
    {
        int pi = partition( arr, low, high);
        
        quick_sort( arr, low, pi - 1);
        quick_sort( arr, pi + 1, high);
    }
}

void sort_array( int arr[], int n)
{
    quick_sort( arr, 0, n - 1);
}

int is_sorted( int arr[], int n)
{
    for ( int i = 0; i < n - 1; i++ )
    {
        if ( arr[i] > arr[i + 1] )
        {
            return 0;
        }
    }
    return 1;
}

int main() {
    const int n = 10000;
    int arr[n];

    for ( int i = 0; i < n; i++ )
    {
        arr[i] = simple_rand();
    }

    sort_array( arr, n);

    return is_sorted( arr, n) ? 0 : 1;
}
