#include <iostream>
#include <memory>
#include <unordered_map>
#include <string>

struct SharedState{
    std::string stereotypeName_;
    std::string thickness_;
    std::string material_;

    SharedState(const std::string& stereotypeName, const std::string& thickness, const std::string& material)
        : stereotypeName_(stereotypeName), thickness_(thickness), material_(material){}
    SharedState(const SharedState& sharedStateIn)
        : stereotypeName_(sharedStateIn.stereotypeName_), thickness_(sharedStateIn.thickness_),
        material_(sharedStateIn.material_) {}

    friend std::ostream& operator << (std::ostream& os, const SharedState& ss){
        return os << "[ " << ss.stereotypeName_ << " , " << ss.thickness_ << " , " << ss.material_  << " ]";
    }
};

struct IndividualState{
    std::string launchDate_;
    std::string portOfCall_;

    IndividualState(const std::string& launchDate, const std::string& portOfCall)
        : launchDate_(launchDate), portOfCall_(portOfCall) {}

    friend std::ostream& operator << (std::ostream& os, const IndividualState& is){
        return os << "[ " << is.launchDate_ << " , " << is.portOfCall_ << " ]";
    }
};

class Flyweight{
public:
    Flyweight(const SharedState* sharedStateIn){
        sharedState_ = std::make_unique<SharedState>(*sharedStateIn);
    }
    Flyweight(const Flyweight& other) : sharedState_(new SharedState(*other.sharedState_)){
        sharedState_ = std::make_unique<SharedState>(*other.sharedState_);
    }
    ~Flyweight(){}

    SharedState* sharedState() const {
        return sharedState_.get();
    }

    void Operation(const IndividualState& individualState) const{
        std::cout << "Flyweight: Displaying shared (" << *sharedState_ << ") and individual (" << individualState << ") state\n.";
    }
private:
    std::unique_ptr<SharedState> sharedState_;
};

class FlyweightFactory{
public:
    FlyweightFactory(std::initializer_list<SharedState> shareStates){
        for (const SharedState& ss : shareStates) {
            this->flyweights_.insert(std::make_pair<std::string, Flyweight>(this->GetKey(ss), Flyweight(&ss)));
        }
    }

    Flyweight GetFlyweight(const SharedState& shared_state)
    {
        std::string key = this->GetKey(shared_state);
        if (this->flyweights_.find(key) == this->flyweights_.end())
        {
            std::cout << "FlyweightFactory: Can't find a flyweight, creating new one.\n";
            this->flyweights_.insert(std::make_pair(key, Flyweight(&shared_state)));
        }
        else
        {
            std::cout << "FlyweightFactory: Reusing existing flyweight.\n";
        }
        return this->flyweights_.at(key);
    }

    void ListFlyweights() const
    {
        size_t count = this->flyweights_.size();
        std::cout << "\nFlyweightFactory: I have " << count << " flyweights:\n";
        for (std::pair<std::string, Flyweight> pair : this->flyweights_)
        {
            std::cout << pair.first << "\n";
        }
    }
private:
    std::unordered_map<std::string, Flyweight> flyweights_;
    std::string GetKey(const SharedState& ss) const{
        return ss.stereotypeName_ + "_" + ss.thickness_ + "_" + ss.material_;
    }
};

void AddHullToDatabase(FlyweightFactory& ff, const std::string& portOfCall, const std::string& launchDate, const std::string& stereotypeName, const std::string& thickness, const std::string& material) {
    std::cout << "Client: Adding specific hull to database.\n";
    const Flyweight& flyweight = ff.GetFlyweight({stereotypeName, thickness, material});
    flyweight.Operation({launchDate, portOfCall});
}

int main() {
    std::unique_ptr<FlyweightFactory> factory;
    factory = std::make_unique<FlyweightFactory>(FlyweightFactory{
        {"Ice Breaker", "30cm", "Hardened Steel"}, {"Pontoon", "1cm", "Reinforced Nylon"}
    });
    std::cout << "First list of flyweights: \n";
    factory->ListFlyweights();
    std::cout << "\n.";

    AddHullToDatabase(*factory, "May 15th", "Santiago", "Ice Breaker", "30cm", "Hardened Steel");
    AddHullToDatabase(*factory, "June 7th", "Puerto Oro", "Galleon", "16cm", "Black Oak");
    for (int i = 0; i < 1000000000; i++)
    {
        AddHullToDatabase(*factory, std::to_string(i), "Santiago", "Ice Breaker", "30cm", "Hardened Steel");
    }

    std::cout << "Second list of flyweights: \n";
    factory->ListFlyweights();
    std::cout << "\n.";

    return 0;
}
