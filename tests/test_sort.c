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

void bubble_sort( int arr[], int n)
{
    for ( int i = 0; i < n - 1; i++ )
    {
        for ( int j = 0; j < n - i - 1; j++ )
        {
            if ( arr[j] > arr[j + 1] )
            {
                swap( &arr[j], &arr[j + 1]);
            }
        }
    }
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
    const int n = 1000;
    int arr[n];

    for ( int i = 0; i < n; i++ )
    {
        arr[i] = simple_rand();
    }

    bubble_sort( arr, n);

    return is_sorted( arr, n) ? 0 : 1;
}
