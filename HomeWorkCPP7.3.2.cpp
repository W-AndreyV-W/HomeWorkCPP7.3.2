#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

class Observer {

public:

    Observer(std::ostream& _out) : out(_out) {}

    virtual void onWarning(const std::string& message) {};

    virtual void onError(const std::string& message) {};

    virtual void onFatalError(const std::string& message) {};

protected:

    virtual void print(const std::string& message) noexcept {

        out << message << std::endl;
    }

private:

    std::ostream& out;
};

class ObserverWarning : public Observer {

public:

    ObserverWarning(std::ostream& _out_console) : Observer(_out_console) {}

    void onWarning(const std::string& message) override {

        Observer::print(message);
    }
};

class ObserverError : public Observer {

public:

    ObserverError(std::ostream& _out_file) : Observer(_out_file) {}

    void onError(const std::string& message) override {

        Observer::print(message);
    }
};

class ObserverFatalError : public Observer {

public:

    ObserverFatalError(std::ostream& _out_console, std::ostream& _out_file) : out(_out_console), Observer(_out_file) {}

    void onFatalError(const std::string& message) override {

        print(message);
    }

protected:

    void print(const std::string& message) noexcept override {

        Observer::print(message);
        out << message << std::endl;
    }

private:

    std::ostream& out;
};

class Subject {

public:

    Subject(std::weak_ptr<Observer> _o_warning, std::weak_ptr<Observer> _o_error, std::weak_ptr<Observer> _o_fatal_error)
        :o_warning(_o_warning), o_error(_o_error), o_fatal_error(_o_fatal_error) {}

    void warning(const std::string& message) const;

    void error(const std::string& message) const;

    void fatalError(const std::string& message) const;

private:

    std::weak_ptr<Observer> o_warning;
    std::weak_ptr<Observer> o_error;
    std::weak_ptr<Observer> o_fatal_error;
};

void Subject::warning(const std::string& message) const {

    if (auto _warning = o_warning.lock()) {

        _warning->onWarning(message);
    }
}

void Subject::error(const std::string& message) const {

    if (auto _error = o_error.lock()) {

        _error->onError(message);
    }
}

void Subject::fatalError(const std::string& message) const {

    if (auto _fatal_error = o_fatal_error.lock()) {

        _fatal_error->onFatalError(message);
    }
}

int main() {

    std::fstream file("out.txt", std::ios::app);

    std::shared_ptr<Observer>s_observerwarning = std::make_shared<ObserverWarning>(ObserverWarning(std::cout));
    std::shared_ptr<Observer>s_observererror = std::make_shared<ObserverError>(ObserverError(file));
    std::shared_ptr<Observer>s_observerfatalerror = std::make_shared<ObserverFatalError>(ObserverFatalError(std::cout, file));

    std::weak_ptr<Observer> observerwarning(s_observerwarning);
    std::weak_ptr<Observer> observererror(s_observererror);
    std::weak_ptr<Observer> observerfatalerror(s_observerfatalerror);

    Subject subject(observerwarning, observererror, observerfatalerror);

    subject.warning("warning");
    subject.error("error");
    subject.fatalError("fatalError");
}