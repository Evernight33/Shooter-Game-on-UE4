#pragma once

class AnimUtils
{
public:
    template <typename T>
    static T* FindNotifyByClass(UAnimSequenceBase* Animation)
    {
        if (Animation == nullptr)
        {
            return nullptr;
        }

        const auto NotifyEvents = Animation->Notifies;

        for (auto NotifyEvent : NotifyEvents)
        {
            auto AnimNotify = Cast<T>(NotifyEvent.Notify);

            if (AnimNotify != nullptr)
            {
                return AnimNotify;
            }
        }

        return nullptr;
    }
};