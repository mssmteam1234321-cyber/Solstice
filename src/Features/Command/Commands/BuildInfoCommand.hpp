#pragma once
//
// Created by vastrakai on 7/29/2024.
//



class BuildInfoCommand : public Command {
public:
    BuildInfoCommand() : Command("buildinfo") {}
    void execute(const std::vector<std::string>& args) override;
    [[nodiscard]] std::vector<std::string> getAliases() const override;
    [[nodiscard]] std::string getDescription() const override;
    [[nodiscard]] std::string getUsage() const override;
};