#pragma once
namespace Engine {
    enum class ResourceType : _uint
    {
        Texture,
        Sound,
        Shader,
        Model,
        Material,
        ComputeShader,
        Animation,
        Effect,
        None
    };

    enum class PreloadState : _uint
    {
        Queued,
        Loading,
        Ready,
        Failed
    };

    struct PreloadOptions
    {
        _int priority = 0;
        _bool isSRGB = false;                 
    };

    struct PreloadKey
    {
        ResourceType type{};
        string levelKey;
        string resourceKey;
        PreloadOptions options{}; 

        bool operator==(const PreloadKey& rhs) const
        {
            return type == rhs.type && levelKey == rhs.levelKey && resourceKey == rhs.resourceKey;
        }
    };

    struct PreloadKeyHash
    {
        size_t operator()(const PreloadKey& value) const noexcept
        {
            size_t hashValue = std::hash<int>()((int)value.type);
            hashValue ^= (std::hash<std::string>()(value.levelKey) + 0x9e3779b97f4a7c15ULL + (hashValue << 6) + (hashValue >> 2));
            hashValue ^= (std::hash<std::string>()(value.resourceKey) + 0x9e3779b97f4a7c15ULL + (hashValue << 6) + (hashValue >> 2));
            return hashValue;
        }
    };

    struct PreloadCompleted
    {
        PreloadKey key;
        PreloadState state;
        string errorMessage;
    };

    struct PreloadTask
    {
        PreloadKey key;
        future<_bool> future;
        PreloadState state = PreloadState::Queued;
        int priority = 0;
        string errorMessage;
        chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
        chrono::steady_clock::time_point loadingTime = chrono::steady_clock::now();
    };
}