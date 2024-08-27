//
// Created by vastrakai on 8/26/2024.
//


#include <random>

class WorkingVM {
public:
    enum class VmOpCode : uint8_t {
        Add = 0x00,
        Sub = 0x01,
        Mul = 0x02,
        Div = 0x03,
        Ret = 0x04,
    };

    struct Instruction {
        VmOpCode OpCode;
        int Operand;
    };

    class Program {
    public:
        void Append(const Instruction& instruction) {
            Instructions.push_back(instruction);
        }

        static std::optional<Program> Parse(const std::string& code) {
            std::string decoded = StringUtils::decode(code);
            int length = decoded.length();
            if (length % 10 != 0) return std::nullopt;

            Program program;
            for (int i = 0; i < length; i += 10) {
                uint8_t opCode;
                int operand;

                std::stringstream ss;
                ss << std::hex << decoded.substr(i, 2);
                ss >> opCode;

                if (opCode > static_cast<uint8_t>(VmOpCode::Ret))
                    return std::nullopt;

                ss.clear();
                ss << std::hex << decoded.substr(i + 2, 8);
                ss >> operand;

                program.Append({ static_cast<VmOpCode>(opCode), operand });
            }

            return program;
        }

        std::string Compile() const {
            std::ostringstream oss;
            for (const auto& instruction : Instructions) {
                oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                    << static_cast<uint8_t>(instruction.OpCode)
                    << std::setw(8) << std::setfill('0') << instruction.Operand;
            }
            return oss.str();
        }

        static std::optional<int> Execute(const std::string& program) {
            auto prog = Parse(program);
            if (!prog) return std::nullopt;

            int result = 0;
            for (const auto& instruction : prog->Instructions) {
                switch (instruction.OpCode) {
                    case VmOpCode::Add:
                        result += instruction.Operand;
                        break;
                    case VmOpCode::Sub:
                        result -= instruction.Operand;
                        break;
                    case VmOpCode::Mul:
                        result *= instruction.Operand;
                        break;
                    case VmOpCode::Div:
                        if (instruction.Operand == 0) return std::nullopt;
                        result /= instruction.Operand;
                        break;
                    case VmOpCode::Ret:
                        return result;
                }
            }

            return result;
        }

    private:
        std::vector<Instruction> Instructions;
    };

    static std::string GenerateProofTask() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist_count(10, 100);
        std::uniform_int_distribution<> dist_operand(-10000, 10000);
        std::uniform_int_distribution<> dist_opCode_even(0, 2);
        std::uniform_int_distribution<> dist_opCode_odd(0, 3);

        Program task;
        int count = dist_count(gen);

        for (int i = 0; i < count; i++) {
            int randomOperand = dist_operand(gen);
            if (randomOperand == 0) randomOperand++;
            int randomOpCode = (i % 2 == 0) ? dist_opCode_even(gen) : dist_opCode_odd(gen);

            task.Append({ static_cast<VmOpCode>(randomOpCode), randomOperand });
        }

        task.Append({ VmOpCode::Ret, 0 });

        std::string compiled = task.Compile();
        compiled = StringUtils::encode(compiled);

        return compiled;
    }

    static int SolveProofTask(const std::string& task) {
        auto prog = Program::Parse(task);
        if (!prog) return -1;
        auto result = Program::Execute(task);
        return result ? *result : -1;
    }
};
