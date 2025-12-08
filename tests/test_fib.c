int fib(int n) {
    if (n <= 1) return n;
    return fib(n-1) + fib(n-2);
}

int fib_iter(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1, c;
    for (int i = 2; i <= n; i++) {
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int main() {
    int n = 30;
    int rec_result = fib(n);
    int iter_result = fib_iter(n);
    
    int ok = 1;
    for (int i = 0; i <= n; i++) {
        if (fib(i) != fib_iter(i)) {
            ok = 0;
        }
    }
    
    return !ok;
}
