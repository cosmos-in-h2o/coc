#ifndef COC_ACTION_HPP
#define COC_ACTION_HPP
#include <coc/def.hpp>
#include <coc/getter.hpp>
#include <coc/ihelp_func.hpp>

namespace coc {
    //Action interface
    struct IAction {
        friend class Actions;

    protected:
        virtual void run(coc::Arguments *arguments) = 0;
        virtual void run(std::list<std::string_view> &opt_tar, Arguments *arguments) = 0;

    public:
        char short_cut{};
        Options *options;
        Values *values;

        virtual std::string_view get_intro() = 0;

        inline IAction *addOption(std::string_view _name, std::string_view _intro, int _num, char _s_name = COC_NULL_CHAR) {
            this->options->addOption(_name, _intro, _num, _s_name);
            return this;
        }
        inline IAction *addValue(std::string_view _name, std::string_view _log, std::string_view _type, std::string_view _intro, std::string_view _def_val = "") {
            this->values->addValue(_name, _log, _type, _intro, _def_val);
            return this;
        }

        IAction(Options *options, Values *values, char short_cut, ParserConfig *config, IParserLog *log);
        IAction();
        virtual ~IAction() = default;
    };

    class HelpAction : public IAction {
    private:
        std::string_view intro;
        IHelpFunc *hf;
        inline void run(coc::Arguments *arguments) override {
            this->hf->run(Getter(this->options, this->values, arguments, true));
        }
        inline void run(std::list<std::string_view> &opt_tar, coc::Arguments *arguments) override {
            this->options->run(opt_tar);
            this->hf->run(Getter(this->options, this->values, arguments));
        }

    public:
        HelpAction(std::string_view intro, IHelpFunc *hf, char short_cut, ParserConfig *config, IParserLog *log);
        ~HelpAction() override;
        inline std::string_view get_intro() override {
            return this->intro;
        }
    };

    class AAction : public IAction {
        friend class Actions;
        friend class Parser;

    protected:
        ActionFun af;

    private:
        inline void run(Arguments *arguments) override {
            this->af(Getter(this->options, this->values, arguments, true));
        }
        inline void run(std::list<std::string_view> &opt_tar, Arguments *arguments) override {
            /*
            * in this step complete:
            * do nothing,only call function
            */

            /*
             * after that
             * call options' run()
             */

            this->options->run(opt_tar);
            this->af(Getter(this->options, this->values, arguments));
        }

    public:
        inline std::string_view get_intro() override {
            return std::string_view{"null"};
        }

        AAction(const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log);
        ~AAction() override;
    };

    class Action : public AAction {
        friend class Actions;
        friend class Parser;

    private:
        std::string_view intro;

    public:
        inline std::string_view get_intro() override {
            return this->intro;
        }

        Action(std::string_view intro, const ActionFun &af, char short_cut, ParserConfig *config, IParserLog *log);
    };

    using GlobalAction = AAction;
}// namespace coc

#endif//COC_ACTION_HPP
