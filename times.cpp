#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <gtk/gtk.h>
#include <cairo.h>

// Global variables to hold the measured times
std::vector<double> bubbleSortTimes;
std::vector<double> selectionSortTimes;
std::vector<double> mergeSortTimes;
std::vector<int> dataSizes = {100, 500, 1000, 5000, 10000};  // Example input sizes

// BubbleSort
void bubbleSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// SelectionSort
void selectionSort(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        std::swap(arr[min_idx], arr[i]);
    }
}

// MergeSort
void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Función para generar datos de prueba
std::vector<int> generateData(int N) {
    std::vector<int> data(N);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, N);
    for (int i = 0; i < N; i++) {
        data[i] = dis(gen);
    }
    return data;
}

// Función para medir tiempos de los algoritmos
void measureTimes() {
    for (int size : dataSizes) {
        std::vector<int> data = generateData(size);

        // BubbleSort
        auto start = std::chrono::high_resolution_clock::now();
        bubbleSort(data);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        bubbleSortTimes.push_back(duration.count());

        // SelectionSort
        data = generateData(size);  // Regenerar los datos para garantizar que no están ordenados
        start = std::chrono::high_resolution_clock::now();
        selectionSort(data);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        selectionSortTimes.push_back(duration.count());

        // MergeSort
        data = generateData(size);
        start = std::chrono::high_resolution_clock::now();
        mergeSort(data, 0, data.size() - 1);
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        mergeSortTimes.push_back(duration.count());
    }
}

// Función para graficar con GTK
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = 600, height = 450;
    int margin = 50;
    int graphWidth = width - 2 * margin;
    int graphHeight = height - 2 * margin;
    
    // Dibujar ejes
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, margin, margin);
    cairo_line_to(cr, margin, height - margin);
    cairo_move_to(cr, margin, height - margin);
    cairo_line_to(cr, width - margin, height - margin);
    cairo_stroke(cr);

    // Máximo valor de tiempo para escalar el gráfico
    double maxTime = std::max({*std::max_element(bubbleSortTimes.begin(), bubbleSortTimes.end()),
                               *std::max_element(selectionSortTimes.begin(), selectionSortTimes.end()),
                               *std::max_element(mergeSortTimes.begin(), mergeSortTimes.end())});

    // Dibujar las líneas para cada algoritmo
    auto drawLine = [&](std::vector<double> times, double r, double g, double b) {
        cairo_set_source_rgb(cr, r, g, b);
        cairo_move_to(cr, margin, height - margin - (times[0] / maxTime) * graphHeight);
        for (size_t i = 1; i < times.size(); i++) {
            int x = margin + (i * graphWidth) / (dataSizes.size() - 1);
            int y = height - margin - (times[i] / maxTime) * graphHeight;
            cairo_line_to(cr, x, y);
        }
        cairo_stroke(cr);
    };

    // Dibujar las líneas de tiempo
    drawLine(bubbleSortTimes, 1, 0, 0);  // BubbleSort en rojo
    drawLine(selectionSortTimes, 0, 1, 0);  // SelectionSort en verde
    drawLine(mergeSortTimes, 0, 0, 1);  // MergeSort en azul

    return FALSE;
}

int main(int argc, char *argv[]) {
    // Medir los tiempos
    measureTimes();

    // GTK para graficar
    GtkWidget *window;
    GtkWidget *darea;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(G_OBJECT(darea), "draw", G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 450);
    gtk_window_set_title(GTK_WINDOW(window), "Gráficas de Tiempos de Ejecución");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
