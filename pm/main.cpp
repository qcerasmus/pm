#include <cstdlib>
#include <ftxui/dom/node.hpp>
#include <iostream>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"
#include "ftxui/screen/string.hpp"

int main()
{
    auto summary = [&]
    {
        auto content = ftxui::vbox({
            ftxui::hbox(
                {ftxui::text(L"- done:   "), ftxui::text(L"3") | ftxui::bold}) |
                color(ftxui::Color::Green),
            ftxui::hbox(
                {ftxui::text(L"- active: "), ftxui::text(L"2") | ftxui::bold}) |
                color(ftxui::Color::RedLight),
            ftxui::hbox(
                {ftxui::text(L"- queue:  "), ftxui::text(L"9") | ftxui::bold}) |
                color(ftxui::Color::Red),
        });
        return window(ftxui::text(L" Summary "), content);
    };

    auto document =
        ftxui::vbox({
            ftxui::hbox({
                summary(),
                summary(),
                summary() | ftxui::flex,
            }),
        });

    document = document | size(ftxui::WIDTH, ftxui::LESS_THAN, 80);

    auto screen = ftxui::Screen::Create(ftxui::Dimension::Full(), ftxui::Dimension::Fit(document));
    ftxui::Render(screen, document);

    std::cout << screen.ToString() << '\0' << std::endl;

    return EXIT_SUCCESS;
}
