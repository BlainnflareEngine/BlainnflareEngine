#include "pch.h"

#include "physics/ContactListenerImpl.h"

#include "subsystems/PhysicsSubsystem.h"

using namespace Blainn;

JPH::ValidateResult ContactListenerImpl::OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                                           JPH::RVec3Arg inBaseOffset,
                                                           const JPH::CollideShapeResult &inCollisionResult)
{
    // BF_INFO("PHYSICS ON CONTACT VALIDATE");

    // Check ordering contract between body 1 and body 2
    bool contract = inBody1.GetMotionType() >= inBody2.GetMotionType()
                    || (inBody1.GetMotionType() == inBody2.GetMotionType() && inBody1.GetID() < inBody2.GetID());
    if (!contract) JPH_BREAKPOINT;

    JPH::ValidateResult result;
    if (mNext != nullptr) result = mNext->OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);
    else result = ContactListener::OnContactValidate(inBody1, inBody2, inBaseOffset, inCollisionResult);

    JPH::RVec3 contact_point = inBaseOffset + inCollisionResult.mContactPointOn1;

    return result;
}

void ContactListenerImpl::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                         const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    // BF_INFO("PHYSICS ON CONTACT ADDED");

    JPH::BodyID bodyID1 = inBody1.GetID();
    JPH::BodyID bodyID2 = inBody2.GetID();

    // Expect bodies to be sorted
    if (!(bodyID1 < bodyID2)) JPH_BREAKPOINT;

    // Insert new manifold into state map
    {
        std::lock_guard lock(mStateMutex);
        JPH::SubShapeIDPair key(bodyID1, inManifold.mSubShapeID1, bodyID2, inManifold.mSubShapeID2);
        if (mState.find(key) != mState.end()) JPH_BREAKPOINT; // Added contact that already existed
        mState[key] = StatePair(inManifold.mBaseOffset, inManifold.mRelativeContactPointsOn1);
    }

    PhysicsEvent event{.eventType = PhysicsEventType::CollisionStarted,
                       .entity1 = PhysicsSubsystem::m_bodyEntityConnections[bodyID1],
                       .entity2 = PhysicsSubsystem::m_bodyEntityConnections[bodyID2]};

    PhysicsSubsystem::s_physicsEventQueue.enqueue(eastl::make_shared<PhysicsEvent>(eastl::move(event)));

    if (mNext != nullptr) mNext->OnContactAdded(inBody1, inBody2, inManifold, ioSettings);
}

void ContactListenerImpl::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                             const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings)
{
    // BF_INFO("PHYSICS ON CONTACT PERSISTED");

    // Expect bodies to be sorted
    if (!(inBody1.GetID() < inBody2.GetID())) JPH_BREAKPOINT;

    // Update existing manifold in state map
    {
        std::lock_guard lock(mStateMutex);
        JPH::SubShapeIDPair key(inBody1.GetID(), inManifold.mSubShapeID1, inBody2.GetID(), inManifold.mSubShapeID2);
        StateMap::iterator i = mState.find(key);
        if (i != mState.end()) i->second = StatePair(inManifold.mBaseOffset, inManifold.mRelativeContactPointsOn1);
        else JPH_BREAKPOINT; // Persisted contact that didn't exist
    }

    if (mNext != nullptr) mNext->OnContactPersisted(inBody1, inBody2, inManifold, ioSettings);
}

void ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    // BF_INFO("PHYSICS ON CONTACT REMOVED");

    JPH::BodyID bodyID1 = inSubShapePair.GetBody1ID();
    JPH::BodyID bodyID2 = inSubShapePair.GetBody2ID();

    // Expect bodies to be sorted
    if (!(bodyID1 < bodyID2)) JPH_BREAKPOINT;

    // Update existing manifold in state map
    {
        std::lock_guard lock(mStateMutex);
        StateMap::iterator i = mState.find(inSubShapePair);
        if (i != mState.end()) mState.erase(i);
        else JPH_BREAKPOINT; // Removed contact that didn't exist
    }

    PhysicsEvent event{.eventType = PhysicsEventType::CollisionEnded,
                       .entity1 = PhysicsSubsystem::m_bodyEntityConnections[bodyID1],
                       .entity2 = PhysicsSubsystem::m_bodyEntityConnections[bodyID2]};

    PhysicsSubsystem::s_physicsEventQueue.enqueue(eastl::make_shared<PhysicsEvent>(eastl::move(event)));

    if (mNext != nullptr) mNext->OnContactRemoved(inSubShapePair);
}

void ContactListenerImpl::SaveState(JPH::StateRecorder &inStream) const
{
    // Write length
    JPH::uint32 length = JPH::uint32(mState.size());
    inStream.Write(length);

    // Get and sort keys
    JPH::Array<JPH::SubShapeIDPair> keys;
    for (const StateMap::value_type &kv : mState)
        keys.push_back(kv.first);
    QuickSort(keys.begin(), keys.end());

    // Write key value pairs
    for (const JPH::SubShapeIDPair &k : keys)
    {
        // Write key
        inStream.Write(k);

        // Write value
        const StatePair &sp = mState.find(k)->second;
        inStream.Write(sp.first);
        inStream.Write(JPH::uint32(sp.second.size()));
        inStream.WriteBytes(sp.second.data(), sp.second.size() * sizeof(JPH::Vec3));
    }
}

void ContactListenerImpl::RestoreState(JPH::StateRecorder &inStream)
{
    // Trace("Restore Contact State");

    // Read length
    JPH::uint32 length;
    if (inStream.IsValidating()) length = JPH::uint32(mState.size());
    inStream.Read(length);

    JPH::Array<JPH::SubShapeIDPair> keys;

    // Clear the state and remember the old state for validation
    StateMap old_state;
    old_state.swap(mState);

    // Prepopulate keys and values with current values if we're validating
    if (inStream.IsValidating())
    {
        // Get and sort keys
        for (const StateMap::value_type &kv : old_state)
            keys.push_back(kv.first);
        JPH::QuickSort(keys.begin(), keys.end());
    }

    // Ensure we have the correct size
    keys.resize(length);

    for (size_t i = 0; i < length; ++i)
    {
        // Read key
        JPH::SubShapeIDPair key;
        if (inStream.IsValidating()) key = keys[i];
        inStream.Read(key);

        StatePair sp;
        if (inStream.IsValidating()) sp = old_state[key];

        // Read offset
        inStream.Read(sp.first);

        // Read num contact points
        JPH::uint32 num_contacts;
        if (inStream.IsValidating()) num_contacts = JPH::uint32(old_state[key].second.size());
        inStream.Read(num_contacts);

        // Read contact points
        sp.second.resize(num_contacts);
        inStream.ReadBytes(sp.second.data(), num_contacts * sizeof(JPH::Vec3));

        // Store the new value
        mState[key] = sp;
    }
}
