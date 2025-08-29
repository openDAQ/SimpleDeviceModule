#include <example_module/common.h>
#include <example_module/example_channel.h>
#include <example_module/example_device.h>
#include <example_module/module_dll.h>
#include <example_module/version.h>
#include <gmock/gmock.h>
#include <opendaq/opendaq.h>

#include <chrono>
#include <thread>

using testing::Contains;
using testing::Each;
using testing::Eq;
using testing::ResultOf;
using testing::SizeIs;

using namespace daq;
using namespace std::chrono_literals;

using ExampleModuleTest = testing::Test;

static ModulePtr CreateModule()
{
    ModulePtr moduleLib;
    createModule(&moduleLib, NullContext());
    return moduleLib;
}

TEST_F(ExampleModuleTest, ExampleModuleLoaded)
{
    const auto instance = Instance();
    
    auto getId = [](const ModulePtr& moduleLib) {
        return moduleLib.getModuleInfo().getId();
    };

    ASSERT_THAT(instance.getModuleManager().getModules(),
                Contains(ResultOf(getId, Eq(EXAMPLE_MODULE_NAME))))
        << "Example module wasn't loaded by openDAQ framework";
}

TEST_F(ExampleModuleTest, CreateModule)
{
    IModule* moduleLib = nullptr;
    ErrCode errCode = createModule(&moduleLib, NullContext());
    ASSERT_TRUE(OPENDAQ_SUCCEEDED(errCode));

    ASSERT_NE(moduleLib, nullptr);
    moduleLib->releaseRef();
}

TEST_F(ExampleModuleTest, ModuleName)
{
    auto moduleLib = CreateModule();
    ASSERT_EQ(moduleLib.getModuleInfo().getName(), "ReferenceDeviceModule")
        << "Example module name mismatches";
}

TEST_F(ExampleModuleTest, ModuleId)
{
    auto moduleLib = CreateModule();
    ASSERT_EQ(moduleLib.getModuleInfo().getId(), EXAMPLE_MODULE_NAME)
        << "Example module name mismatches";
}

TEST_F(ExampleModuleTest, VersionCorrect)
{
    auto moduleLib = CreateModule();
    auto version = moduleLib.getModuleInfo().getVersionInfo();

    ASSERT_EQ(version.getMajor(), EXAMPLE_MODULE_MAJOR_VERSION);
    ASSERT_EQ(version.getMinor(), EXAMPLE_MODULE_MINOR_VERSION);
    ASSERT_EQ(version.getPatch(), EXAMPLE_MODULE_PATCH_VERSION);
}

TEST_F(ExampleModuleTest, EnumerateDevices)
{
    auto moduleLib = CreateModule();

    ListPtr<IDeviceInfo> devices;
    ASSERT_NO_THROW(devices = moduleLib.getAvailableDevices());

    auto getConnectionString = [](const DeviceInfoPtr device) {
        return device.getConnectionString();
    };
    ASSERT_THAT(devices, Each(ResultOf(getConnectionString, Eq("example://device"))))
        << "Example module doesn't contain example devices";
}

TEST_F(ExampleModuleTest, CreateDevice)
{
    auto moduleLib = CreateModule();

    DevicePtr device;
    ASSERT_NO_THROW(device = moduleLib.createDevice("example://device", nullptr));

    DeviceInfoPtr info;
    ASSERT_NO_THROW(info = device.getInfo());
    ASSERT_EQ(info.getName(), "Example Device");
    ASSERT_EQ(info.getManufacturer(), "openDAQ");
    ASSERT_EQ(info.getModel(), "Reference device");
    ASSERT_EQ(info.getSerialNumber(), "ExampleDB1234");

    DeviceTypePtr type;
    ASSERT_NO_THROW(type = info.getDeviceType());
    ASSERT_EQ(type.getId(), "example_dev");
    ASSERT_EQ(type.getName(), "Example device");
    ASSERT_EQ(type.getDescription(), "Example device");
    ASSERT_EQ(type.getConnectionStringPrefix(), "example");
}

TEST_F(ExampleModuleTest, CreateOnlyOneDevice)
{
    auto moduleLib = CreateModule();

    DevicePtr device;
    ASSERT_NO_THROW(device = moduleLib.createDevice("example://device", nullptr))
        << "Create the first device must not fail";
    ASSERT_THROW(device = moduleLib.createDevice("example://device", nullptr), std::runtime_error)
        << "Only one example device is allowed";
}

TEST_F(ExampleModuleTest, CreateDeviceInvalidConnection)
{
    auto moduleLib = CreateModule();

    ASSERT_THROW(DevicePtr device = moduleLib.createDevice("invalid://device", nullptr), std::runtime_error);
}

TEST_F(ExampleModuleTest, DeviceNameAndDeviceInfoNameIsTheSame)
{
    auto moduleLib = CreateModule();

    DevicePtr device;
    ASSERT_NO_THROW(device = moduleLib.createDevice("example://device", nullptr))
        << "Create example device must not fail";

    DeviceInfoPtr info;
    ASSERT_NO_THROW(info = device.getInfo());

    ASSERT_EQ(device.getName(), info.getName())
        << "Device name should be the same as info";
}

TEST_F(ExampleModuleTest, GetDeviceType)
{
    auto moduleLib = CreateModule();

    DictPtr<IString, IDeviceType> deviceTypes;
    ASSERT_NO_THROW(deviceTypes = moduleLib.getAvailableDeviceTypes());

    StringPtr key = "example_dev";
    ASSERT_TRUE(deviceTypes.hasKey(key));
    ASSERT_EQ(deviceTypes.get(key).getId(), key);
}

TEST_F(ExampleModuleTest, GetValueSignals)
{
    auto moduleLib = CreateModule();
    auto device = moduleLib.createDevice("example://device", nullptr);

    ListPtr<ISignal> signals;
    ASSERT_NO_THROW(signals = device.getSignalsRecursive());

    SizeT count = signals.getCount();
    ASSERT_GT(count, 0u) << "Signals list must not be empty";

    for (const auto& signal : signals)
    {
        auto id = signal.getGlobalId();

        DataDescriptorPtr signalDescriptor;
        ASSERT_NO_THROW(signalDescriptor = signal.getDescriptor())
            << id << " get signal descriptor must not fail";
        ASSERT_TRUE(signalDescriptor.assigned())
            << id << " signal descriptor is not assigned";

        SampleType signalSampleType;
        ASSERT_NO_THROW(signalSampleType = signalDescriptor.getSampleType()) 
            << id << " get signal descriptor failed";
        ASSERT_EQ(signalSampleType, SampleType::Float64)
            << id << " float 64 signal sample type is expected";

        UnitPtr signalUnit;
        ASSERT_NO_THROW(signalUnit = signalDescriptor.getUnit()) 
            << id << " get signal descriptor failed";
        ASSERT_TRUE(signalUnit.assigned())
            << id << " signal unit is not assigned";

        ASSERT_EQ(signalUnit.getSymbol(), "V")
            << id << " signal unit symbol mismatches";
        ASSERT_EQ(signalUnit.getId(), -1)
            << id << " signal unit ID mismatches";
        ASSERT_EQ(signalUnit.getName(), "volts")
            << id << " signal unit name mismatches";
        ASSERT_EQ(signalUnit.getQuantity(), "voltage")
            << id << " signal unit quantity mismatches";
    }
}

TEST_F(ExampleModuleTest, GetDomainSignal)
{
    auto moduleLib = CreateModule();
    auto device = moduleLib.createDevice("example://device", nullptr);

    ListPtr<ISignal> signals;
    ASSERT_NO_THROW(signals = device.getSignalsRecursive());

    SizeT count = signals.getCount();
    ASSERT_GT(count, 0u) << "Signals list must not be empty";

    for (const auto& signal : signals)
    {
        SignalPtr domainSignal;
        ASSERT_NO_THROW(domainSignal = signal.getDomainSignal())
            << signal.getGlobalId()<< " get domain signal must not throw";
        ASSERT_NO_THROW(domainSignal.assigned())
            << signal.getGlobalId() << " domain signal is not assigned";

        auto id = domainSignal.getGlobalId();

        DataDescriptorPtr domainDescriptor;
        ASSERT_NO_THROW(domainDescriptor = domainSignal.getDescriptor()) 
            << id << " get domain descriptor failed";
        ASSERT_TRUE(domainDescriptor.assigned()) 
            << id << " signal domain is not assigned";
        ASSERT_EQ(domainDescriptor.getSampleType(), SampleType::Int64)
            << id << " domain signal sample type mismatches";

        UnitPtr domainUnit;
        ASSERT_NO_THROW(domainUnit = domainDescriptor.getUnit()) 
            << id << " get signal descriptor failed";
        ASSERT_TRUE(domainUnit.assigned())
            << id << " signal unit is not assigned";

        ASSERT_EQ(domainUnit.getSymbol(), "s") 
            << id << " domain signal unit symbol mismatches";
        ASSERT_EQ(domainUnit.getId(), -1)
            << id << " domain signal unit ID mismatches" ;
        ASSERT_EQ(domainUnit.getName(), "seconds")
            << id << " domain signal unit name mismatches";
        ASSERT_EQ(domainUnit.getQuantity(), "time")
            << id << " domain signal unit quantity mismatches";

        RatioPtr tickResolution;
        ASSERT_NO_THROW(tickResolution = domainDescriptor.getTickResolution()) 
            << id << " get domain signal tick resolution failed";
        ASSERT_TRUE(tickResolution.assigned())
            << id << " domain signal tick resolution is not assigned";
        ASSERT_EQ(tickResolution, Ratio(1, 1000000))
            << id << " domain signal tick resolution mismatches";

        DataRulePtr dataRule;
        ASSERT_NO_THROW(dataRule = domainDescriptor.getRule()) 
            << id << " get domain signal rule failed";
        ASSERT_TRUE(dataRule.assigned())
            << id << " domain signal data rule is not assigned";

        ASSERT_EQ(dataRule.getType(), DataRuleType::Linear) 
            << id << " domain signal data rule mismatches";
        StringPtr deltaKey = "delta";
        ASSERT_EQ(dataRule.getParameters().get(deltaKey), 1000) 
            << id << " domain signal data rule \"" << deltaKey << "\" parameter mismatches";
        StringPtr keyStart = "start";
        ASSERT_EQ(dataRule.getParameters().get(keyStart), 0) 
            << id << " domain signal data rule \"" << keyStart << "\" parameter mismatches";
    }
}

TEST_F(ExampleModuleTest, StreamReader)
{
    auto moduleLib = CreateModule();
    auto device = moduleLib.createDevice("example://device", nullptr);

    ListPtr<ISignal> signals;
    ASSERT_NO_THROW(signals = device.getSignalsRecursive());

    SizeT count = signals.getCount();
    ASSERT_GT(count, 0u) << "Signals list must not be empty";

    double buffer[100];
    for (const auto& signal : signals)
    {
        auto reader = StreamReader<double>(signal);
        auto capacity = sizeof(buffer) / sizeof(double);
        reader.read(buffer, &capacity, std::chrono::milliseconds(1s).count());
        EXPECT_GT(count, 0u) << signal.getGlobalId() << " no samples have been read";
    }
}
