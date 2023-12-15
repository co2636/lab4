#include <gtk/gtk.h>

// 계산기 구조체 정의
typedef struct {
    GtkWidget *entry; // 텍스트 입력을 위한 엔트리 위젯
} Calculator;

// 숫자 버튼 눌렀을 때의 콜백 함수
static void on_button_clicked(GtkButton *button, gpointer data) {
    const gchar *text = gtk_button_get_label(button);
    gchar *current_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(((Calculator *)data)->entry)));
    gchar *new_text = g_strconcat(current_text, text, NULL);
    gtk_entry_set_text(GTK_ENTRY(((Calculator *)data)->entry), new_text);
    g_free(current_text);
    g_free(new_text);
}

// 연산자 버튼 눌렀을 때의 콜백 함수
static void on_operator_clicked(GtkButton *button, gpointer data) {
    const gchar *operator = gtk_button_get_label(button);
    gchar *current_text = g_strdup(gtk_entry_get_text(GTK_ENTRY(((Calculator *)data)->entry)));
    gchar *new_text = g_strconcat(current_text, operator, NULL);
    gtk_entry_set_text(GTK_ENTRY(((Calculator *)data)->entry), new_text);
    g_free(current_text);
    g_free(new_text);
}

// 계산 버튼 눌렀을 때의 콜백 함수
static void on_calculate_clicked(GtkButton *button, gpointer data) {
    const gchar *expression = gtk_entry_get_text(GTK_ENTRY(((Calculator *)data)->entry));
    gdouble result = 0.0;
    if (g_strrstr(expression, "+")) {
        gchar **tokens = g_strsplit_set(expression, "+", 2);
        result = g_ascii_strtod(tokens[0], NULL) + g_ascii_strtod(tokens[1], NULL);
        g_strfreev(tokens);
    } else if (g_strrstr(expression, "-")) {
        gchar **tokens = g_strsplit_set(expression, "-", 2);
        result = g_ascii_strtod(tokens[0], NULL) - g_ascii_strtod(tokens[1], NULL);
        g_strfreev(tokens);
    } else if (g_strrstr(expression, "x")) {
        gchar **tokens = g_strsplit_set(expression, "x", 2);
        result = g_ascii_strtod(tokens[0], NULL) * g_ascii_strtod(tokens[1], NULL);
        g_strfreev(tokens);
    } else if (g_strrstr(expression, "/")) {
        gchar **tokens = g_strsplit_set(expression, "/", 2);
        if (g_ascii_strtod(tokens[1], NULL) != 0.0) {
            result = g_ascii_strtod(tokens[0], NULL) / g_ascii_strtod(tokens[1], NULL);
        } else {
            g_print("Error: Division by zero\n");
        }
        g_strfreev(tokens);
    }
    gchar result_str[50];
    g_snprintf(result_str, sizeof(result_str), "%.2f", result);
    gtk_entry_set_text(GTK_ENTRY(((Calculator *)data)->entry), result_str);
}

// 초기화 버튼 눌렀을 때의 콜백 함수
static void on_clear_clicked(GtkButton *button, gpointer data) {
    gtk_entry_set_text(GTK_ENTRY(((Calculator *)data)->entry), "");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // 윈도우 생성
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "계산기");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // 계산기 구조체 초기화
    Calculator calculator;
    calculator.entry = gtk_entry_new();
    gtk_entry_set_alignment(GTK_ENTRY(calculator.entry), 1.0);
    gtk_entry_set_max_length(GTK_ENTRY(calculator.entry), 30);
    gtk_widget_set_size_request(calculator.entry, -1, 50);
    gtk_editable_set_editable(GTK_EDITABLE(calculator.entry), FALSE);

    // 그리드 및 버튼 생성
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    const gchar *button_labels[] = {
        "7", "8", "9", "+",
        "4", "5", "6", "-",
        "1", "2", "3", "x",
        "0", ".", "/", "=",
        "C"
    };

    for (int i = 0; i < G_N_ELEMENTS(button_labels); i++) {
        GtkWidget *button = gtk_button_new_with_label(button_labels[i]);
        gtk_widget_set_size_request(button, 70, 70);
        if (g_strrstr(button_labels[i], "+-x/")) {
            g_signal_connect(button, "clicked", G_CALLBACK(on_operator_clicked), &calculator);
        } else if (g_strrstr(button_labels[i], "=")) {
            g_signal_connect(button, "clicked", G_CALLBACK(on_calculate_clicked), &calculator);
        } else if (g_strrstr(button_labels[i], "C")) {
            g_signal_connect(button, "clicked", G_CALLBACK(on_clear_clicked), &calculator);
        } else {
            g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), &calculator);
        }
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, i / 4, 1, 1);
    }

    gtk_grid_attach(GTK_GRID(grid), calculator.entry, 0, 4, 4, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

