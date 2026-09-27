#include "icm20948_driver.hpp"

#include <cassert>
#include <cstdint>
#include <span>
#include <vector>

namespace {

class RecordingSpiBus final : public aerocam::SpiBus {
public:
    void select() override { selected = true; }
    void deselect() override { selected = false; }

    void transfer(std::span<const std::uint8_t> tx,
                  std::span<std::uint8_t> rx) override {
        assert(selected);
        assert(tx.size() == rx.size());
    }

    void write(std::span<const std::uint8_t> tx) override {
        assert(selected);
        writes.emplace_back(tx.begin(), tx.end());
    }

    bool selected = false;
    std::vector<std::vector<std::uint8_t>> writes;
};

} // namespace

int main() {
    RecordingSpiBus spi;
    aerocam::ICM20948_Driver driver(spi);

    assert(driver.initialize());
    assert(!spi.selected);
    assert(!spi.writes.empty());

    // Initialization must begin by selecting bank 0, not recursively trying
    // to select a bank forever.
    assert(spi.writes.front().size() == 2);
    assert(spi.writes.front()[0] == 0x7F);
    assert(spi.writes.front()[1] == 0x00);
}
