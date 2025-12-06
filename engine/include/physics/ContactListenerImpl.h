#pragma once

namespace Blainn
{

// Tests the contact listener callbacks
class ContactListenerImpl : public JPH::ContactListener
{
public:
    // See: ContactListener
    virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                                  JPH::RVec3Arg inBaseOffset,
                                                  const JPH::CollideShapeResult &inCollisionResult) override;
    virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
    virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                    const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override;
    virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;

    // Saving / restoring state for replay
    void SaveState(JPH::StateRecorder &inStream) const;
    void RestoreState(JPH::StateRecorder &inStream);

    // Draw the current contact state
    // void					DrawState();

    // Ability to defer to the next contact listener after this one handles the callback
    void SetNextListener(ContactListener *inListener)
    {
        mNext = inListener;
    }

private:
    // Map that keeps track of the current state of contacts based on the contact listener callbacks
    using StatePair = eastl::pair<JPH::RVec3, JPH::ContactPoints>;
    using StateMap = JPH::UnorderedMap<JPH::SubShapeIDPair, StatePair>;
    JPH::Mutex mStateMutex;
    StateMap mState;

    ContactListener *mNext = nullptr;
};

} // namespace Blainn