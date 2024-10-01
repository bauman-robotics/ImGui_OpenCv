#include "main.h"

void View_Group_1() {

    static std::vector<std::string> items1 = { "Item 1", "Item 2", "Item 3" };
    static std::vector<const char*> items1_cstr;
    static int item_current1 = 0;

    ImGui::BeginChild("Group 1", ImVec2(300, 150), true);
    ImGui::Text("Группа 1");

    // Кнопка для добавления элемента
    if (ImGui::Button("Добавить элемент"))
    {
        // Добавление нового элемента в листбокс
        items1.push_back("Новый элемент " + std::to_string(items1.size() + 1));
        items1_cstr.clear();
        for (const auto& item : items1)
        {
            items1_cstr.push_back(item.c_str());
        }
    }

    // Кнопка для очистки списка
    ImGui::SameLine();
    if (ImGui::Button("Очистить список"))
    {
        // Очистка списка
        items1.clear();
        items1_cstr.clear();
    }

    static bool checkbox = false;
    ImGui::Checkbox("Check Me", &checkbox);

    // if (ImGui::IsItemEdited())
    // {
    //     if (checkbox) {
    //         sprintf(var.socket.send.message, "%s", "HEX");          
    //         var.socket.send.need_to_be_sended.store(1);    
    //     } else {
    //         sprintf(var.socket.send.message, "%s", "ASCII");          
    //         var.socket.send.need_to_be_sended.store(1); 
    //     }
    // }

    ImGui::ListBox("Листбокс 1", &item_current1, items1_cstr.data(), items1_cstr.size());
    ImGui::EndChild();

}