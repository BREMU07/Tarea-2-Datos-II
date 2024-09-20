#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <gtk/gtk.h>
#include <cairo.h>

using namespace std;

// Variables globales para almacenar los tiempos medidos
vector<double> bubbleSortTimes;
vector<double> selectionSortTimes;
vector<double> mergeSortTimes;
vector<double> linkedListSearchTimes;
vector<double> bstInsertTimes;
vector<int> dataSizes = {100, 500, 1000, 5000, 10000};

// BubbleSort
void bubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// SelectionSort
void selectionSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(arr[min_idx], arr[i]);
    }
}

// MergeSort
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Lista enlazada ordenada
class Node {
public:
    int data;
    Node* next;
    Node(int val) : data(val), next(nullptr) {}
};

class SortedLinkedList {
public:
    Node* head;
    SortedLinkedList() : head(nullptr) {}
    void insert(int val) {
        Node* newNode = new Node(val);
        if (!head || head->data >= val) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* current = head;
            while (current->next && current->next->data < val) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }
    bool search(int val) {
        Node* current = head;
        while (current && current->data <= val) {
            if (current->data == val) return true;
            current = current->next;
        }
        return false;
    }
};

// Árbol binario de búsqueda
class BSTNode {
public:
    int data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(int val) : data(val), left(nullptr), right(nullptr) {}
};

class BinarySearchTree {
public:
    BSTNode* root;
    BinarySearchTree() : root(nullptr) {}
    void insert(int val) {
        root = insertRec(root, val);
    }
private:
    BSTNode* insertRec(BSTNode* node, int val) {
        if (!node) return new BSTNode(val);
        if (val < node->data) node->left = insertRec(node->left, val);
        else node->right = insertRec(node->right, val);
        return node;
    }
};

// Generar datos de prueba
vector<int> generateData(int N) {
    vector<int> data(N);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, N);
    for (int i = 0; i < N; i++) data[i] = dis(gen);
    return data;
}

// Medir tiempos de ordenamiento
void measureTimes() {
    for (int size : dataSizes) {
        vector<int> data = generateData(size);

        // BubbleSort
        auto start = chrono::high_resolution_clock::now();
        bubbleSort(data);
        auto end = chrono::high_resolution_clock::now();
        bubbleSortTimes.push_back(chrono::duration<double>(end - start).count());

        // SelectionSort
        data = generateData(size);
        start = chrono::high_resolution_clock::now();
        selectionSort(data);
        end = chrono::high_resolution_clock::now();
        selectionSortTimes.push_back(chrono::duration<double>(end - start).count());

        // MergeSort
        data = generateData(size);
        start = chrono::high_resolution_clock::now();
        mergeSort(data, 0, data.size() - 1);
        end = chrono::high_resolution_clock::now();
        mergeSortTimes.push_back(chrono::duration<double>(end - start).count());
    }
}

// Medir tiempos de LinkedList y BST
void measureLinkedListAndBSTTimes() {
    for (int size : dataSizes) {
        SortedLinkedList list;
        vector<int> data = generateData(size);
        for (int val : data) list.insert(val);

        auto start = chrono::high_resolution_clock::now();
        list.search(data[size / 2]);
        auto end = chrono::high_resolution_clock::now();
        linkedListSearchTimes.push_back(chrono::duration<double>(end - start).count());

        BinarySearchTree bst;
        start = chrono::high_resolution_clock::now();
        for (int val : data) bst.insert(val);
        end = chrono::high_resolution_clock::now();
        bstInsertTimes.push_back(chrono::duration<double>(end - start).count());
    }
}

// Evento de dibujo para GTK
gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    int width = 600, height = 450, margin = 50;
    int graphWidth = width - 2 * margin, graphHeight = height - 2 * margin;

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_move_to(cr, margin, height - margin);
    cairo_line_to(cr, margin, margin);
    cairo_move_to(cr, margin, height - margin);
    cairo_line_to(cr, width - margin, height - margin);
    cairo_stroke(cr);

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_font_size(cr, 10);
    for (int i = 0; i <= 5; i++) {
        double y = height - margin - i * (graphHeight / 5);
        string label = to_string(i * 2) + "s";
        cairo_move_to(cr, 5, y);
        cairo_show_text(cr, label.c_str());
    }

    for (size_t i = 0; i < dataSizes.size(); i++) {
        int x = margin + (i * graphWidth) / (dataSizes.size() - 1);
        string label = to_string(dataSizes[i]);
        cairo_move_to(cr, x, height - margin + 20);
        cairo_show_text(cr, label.c_str());
    }

    double maxTime = max({*max_element(bubbleSortTimes.begin(), bubbleSortTimes.end()),
                          *max_element(selectionSortTimes.begin(), selectionSortTimes.end()),
                          *max_element(mergeSortTimes.begin(), mergeSortTimes.end()),
                          *max_element(linkedListSearchTimes.begin(), linkedListSearchTimes.end()),
                          *max_element(bstInsertTimes.begin(), bstInsertTimes.end())});

    auto drawLine = [&](vector<double> times, double r, double g, double b) {
        cairo_set_source_rgb(cr, r, g, b);
        cairo_move_to(cr, margin, height - margin - (times[0] / maxTime) * graphHeight);
        for (size_t i = 1; i < times.size(); i++) {
            int x = margin + (i * graphWidth) / (dataSizes.size() - 1);
            int y = height - margin - (times[i] / maxTime) * graphHeight;
            cairo_line_to(cr, x, y);
        }
        cairo_stroke(cr);
    };

    drawLine(bubbleSortTimes, 1, 0, 0);
    drawLine(selectionSortTimes, 0, 1, 0);
    drawLine(mergeSortTimes, 0, 0, 1);
    drawLine(linkedListSearchTimes, 1, 1, 0);
    drawLine(bstInsertTimes, 1, 0, 1);

    int legendX = width - margin - 120, legendY = margin, lineHeight = 15;
    cairo_set_source_rgb(cr, 0.9, 0.9, 0.9);
    cairo_rectangle(cr, legendX, legendY, 110, 90);
    cairo_fill(cr);

    auto drawLegendItem = [&](const char* label, double r, double g, double b, int index) {
        cairo_set_source_rgb(cr, r, g, b);
        cairo_rectangle(cr, legendX + 10, legendY + 10 + index * lineHeight, 10, 10);
        cairo_fill(cr);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_move_to(cr, legendX + 30, legendY + 20 + index * lineHeight);
        cairo_show_text(cr, label);
    };

    drawLegendItem("BubbleSort", 1, 0, 0, 0);
    drawLegendItem("SelectionSort", 0, 1, 0, 1);
    drawLegendItem("MergeSort", 0, 0, 1, 2);
    drawLegendItem("LinkedList Search", 1, 1, 0, 3);
    drawLegendItem("BST Insert", 1, 0, 1, 4);

    return FALSE;
}

int main(int argc, char *argv[]) {
    measureTimes();
    measureLinkedListAndBSTTimes();

    GtkWidget *window, *darea;
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
