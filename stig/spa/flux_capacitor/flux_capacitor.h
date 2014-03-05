/* <stig/spa/flux_capacitor/flux_capacitor.h>

     'This is what makes time travel possible!'
         -- Dr. Emmett 'Doc' Brown

   An in-memory structure for maintaining version control over
   a multiply indexed fat graph in multiple, potentially
   conflicting timelines.

   Copyright 2010-2014 Stig LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <mutex>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#include <base/no_copy_semantics.h>
#include <base/past_end_error.h>
#include <base/peekable.h>
#include <base/stl_utils.h>
#include <server/half_latch.h>
#include <stig/context_base.h>
#include <stig/indy/key.h>
#include <stig/key_generator.h>
#include <stig/shared_enum.h>
#include <stig/spa/flux_capacitor/kv.h>
#include <stig/spa/flux_capacitor/sync.h>
#include <stig/spa/flux_capacitor/tetris_piece.h>
#include <stig/var/new_sabot.h>
#include <stig/var/sabot_to_var.h>

namespace Stig {

  namespace Spa {

    class TClosure;

    namespace FluxCapacitor {

      /*  ----------------------------------------------------------------------
       *  Forward declarations.
       */

      /* These classes form the points-of-view in the causality graph.
         This is a classic 6-class system for representing a tree,
         allowing specializations at the root, middle, and leaf levels.
         There is an inheritance diamond: TPov -> TParentPov, TChildPov -> TSharedPov. */
      class TPov;
      class TParentPov;
      class TChildPov;
      class TGlobalPov;
      class TPrivatePov;
      class TSharedPov;

      /* These classes form the events in the causality graph.
         This is a classic 7-class system for representing a graph,
         allowing specializations at the root, middle, and leaf levels.
         There is an inheritance diamond: TEvent -> TCause, TEffect -> TUpdate. */
      class TEvent;
      class TCause;
      class TEffect;
      class TLink;
      class TRoot;
      class TLeaf;
      class TUpdate;

      /* These classes form the indexing and versioning subsystem.
         The 'key' here is a KV.  We keep these
         indices separately. */
      template <typename TKey>
      class TIndex;
      class TKVIndex;
      template <typename TKey>
      class TTour;
      template <typename TKey>
      class TVersion;
      template <typename TKey>
      class TCluster;

      /* And the rest. */
      class TContext;

      typedef std::function<bool (TContext &ctx)> TAssertChecker;

      /*  ----------------------------------------------------------------------
       *  TPov.
       */

      /* A collector of updates and a frame of reference for the flow of time.
         This is the base class for all classes in the pov tree.
         The tree, as a whole, contains the causality graph. */
      class TPov {
        NO_COPY_SEMANTICS(TPov);
        public:

        /* Iterates forward through the trailing updates in a pov. */
        class TTrailingUpdateCursor {
          NO_COPY_SEMANTICS(TTrailingUpdateCursor);
          public:

          /* The type of thing we iterate. */
          typedef TUpdate TItem;

          /* Begin at first trailing update in the given pov.
             Passing a pov with no trailing updates will yield an exhausted iterator.
             Passing a null pov will also yield an exhausted iterator. */
          explicit TTrailingUpdateCursor(const TPov *pov)
              : TrailingUpdate(pov ? pov->FirstTrailingUpdate : 0) {}

          /* True iff. we're not yet exhausted. */
          operator bool() const {
            assert(this);
            return TrailingUpdate != 0;
          }

          /* The current trailing update.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TUpdate *operator*() const {
            assert(this);
            assert(TrailingUpdate);
            return TrailingUpdate;
          }

          /* The current trailing update.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TUpdate *operator->() const {
            assert(this);
            assert(TrailingUpdate);
            return TrailingUpdate;
          }

          /* Advance beyond the current trailing update.
             Calling this function on an exhausted iterator is an error. */
          TTrailingUpdateCursor &operator++();

          private:

          /* The current trailing update.
             Null if we're exhausted. */
          TUpdate *TrailingUpdate;

        };  // TTrailingUpdateCursor

        /* Delete all trailing updates in this pov. Use this before calling
           the destructor on this base class. */
        void DeleteAllTrailingUpdate();

        /* The root of the causality graph.  Every pov in the tree sees the
           same root.  Never null. */
        TRoot *GetRoot() {
          assert(this);
          return Root;
        }

        /* Our index of all KVs being updated. */
        TKVIndex *GetKVIndex() const {
          assert(this);
          return KVIndex;
        }

        /* TODO */
        const TSync &GetSync() const {
          assert(this);
          return Sync;
        }

        /* A blocking file descriptor on which to wait for trailing updates.
           This is an epoll handle.  You can wait for it but don't try to use it
           for reading or writing. */
        int GetTrailingWaitHandle() const {
          assert(this);
          return PipeHandles[0];
        }

        /* Our pointer to the global pov. */
        TGlobalPov *GetGlobalPov() const {
          assert(this);
          return GlobalPov;
        }

        /* True iff. the given pov lies between this pov and the global.
           For the purposes of this function, both this pov and the global are
           considered ancestors of this pov. */
        bool HasAncestor(const TPov *ancestor) const;

        /* The child of this pov which leads toward the given target.
           If the target is not a descendant of this pov, return null. */
        const TPov *TryGetChildPov(const TPov *target) const;

        /* The parent of this pov.
           If this pov has no parent, return null. */
        virtual TParentPov *TryGetParentPov() const = 0;

        protected:

        /* Constructs our indices. */
        TPov();

        /* Also destroys our indices. Any final class should call
           DeleteAllTrailingUpdate before calling this destructor. */
        virtual ~TPov();

        /* An index of all the KVs being updated in this pov.  Never null.
           We return by out-parameter in order to take advantage of function
           overloading in the template-based classes. */
        void GetIndex(TIndex<TKV> *&out) const;

        /* Mark this pov as having failed, meaning that it can no longer
           progress in time.  This only has meaning for psov with a parent. */
        virtual void Fail() = 0;

        /* See accessor.  This must be set in the constructor of our
           descendant class.  We avoid taking it as a construction parameter
           here because this class is the apex of an inheritance diamond. */
        TRoot *Root;

        /* A pointer to our global pov. */
        TGlobalPov *GlobalPov;

        /* The first and last updates in this pov which have no causes within this
           pov.  Such updates are candidates for promotion (in the chase of a child
           pov) or culling (in the case of a global pov). */
        TUpdate *FirstTrailingUpdate, *LastTrailingUpdate;

        /* See accessor. */
        TKVIndex *KVIndex;

        /* Handles to both ends of a pipe().  Used to signal when the pov has
           at least one trailing update.  [0] is read-only and [1] is write-only. */
        int PipeHandles[2];

        /* TODO */
        TSync Sync;

        /* FirstTrailingUpdate, LastTrailingUpdate, and PipeHandles, Sync. */
        friend class TUpdate;

        /* GetIndex<>(). */
        friend class TCluster<TKV>;

        /* GetIndex<>(). */
        friend class TTour<TKV>;

      };  // TPov

      /*  ----------------------------------------------------------------------
       *  TParentPov.
       */

      /* The base class of all points-of-view which can have children. */
      class TParentPov
          : public virtual TPov {
        NO_COPY_SEMANTICS(TParentPov);
        public:

        /* Iterates forward through the children of a parent pov. */
        class TChildPovCursor {
          NO_COPY_SEMANTICS(TChildPovCursor);
          public:

          /* The type of thing we iterate. */
          typedef TChildPov TItem;

          /* Begin at the first child of the given parent pov.
             Passing a parent with no children will yield an exhausted iterator.
             Passing a null parent will also yield an exhausted iterator. */
          explicit TChildPovCursor(const TParentPov *parent_pov)
              : ChildPov(parent_pov ? parent_pov->FirstChildPov : 0) {}

          /* True iff. we're not yet exhausted. */
          operator bool() const {
            assert(this);
            return ChildPov != 0;
          }

          /* The current child.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TChildPov *operator*() const {
            assert(this);
            assert(ChildPov);
            return ChildPov;
          }

          /* The current child.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TChildPov *operator->() const {
            assert(this);
            assert(ChildPov);
            return ChildPov;
          }

          /* Advance beyond the current child.
             Calling this function on an exhausted iterator is an error. */
          TChildPovCursor &operator++();

          private:

          /* The current child.
             Null if we're exhausted. */
          TChildPov *ChildPov;

        };  // TChildPovCursor

        /* TODO */
        int GetTetrisWaitHandle() {
          assert(this);
          return TetrisWaitHandle;
        }

        /* Promote, fail, or ignore all the trailing updates in our child psov.
           The timeout is in milliseconds.  A zero means not to wait at all.  A negative value means to wait forever.
           If the game is played, the function returns true.  If the timeout expired, it returns false. */
        bool PlayTetris(int timeout);

        protected:

        /* We begin with no children. */
        TParentPov();

        /* Also destroys all children of this pov. */
        virtual ~TParentPov();

        private:

        /* Each child must nab this mutex before joining to or parting from this parent. */
        std::recursive_mutex ChildListMutex;

        /* The first and last children of this parent. */
        TChildPov *FirstChildPov, *LastChildPov;

        /* The number of child psov attached to us. */
        int ChildPovCount;

        /* See accessor. */
        int TetrisWaitHandle;

        /* ChildListMutex, FirstChildPov, and LastChildPov. */
        friend class TChildPov;

      };  // TParentPov

      /*  ----------------------------------------------------------------------
       *  TChildPov.
       */

      /* The base class of all points-of-view which have parents. */
      class TChildPov
          : public virtual TPov {
        NO_COPY_SEMANTICS(TChildPov);
        public:

        /* Describes the flow of updates from this child pov toward its parent.  You may
           determine the flow state calling GetFlowState().  A child pov starts in the
           'Running' state and remains so until you call Pause() or until one or more of
           its updates fail to promote. */
        enum TFlowState {

          /* This the normal state, indicating that any updates trailing in
             this child pov can be promoted to its parent.  You may change from
             this state to 'Paused' whenever you like by calling Pause(). */
          Running,

          /* The flow from this child pov has been temporarily stopped, meaning that no
             updates can be promoted from it.  You may change from this state to 'Running'
             whenever you like by calling Unpause(). */
          Paused,

          /* The flow from this child pov has been permanently stopped due to
             irreconcilability with its parent.  There is no escape from this state. */
          Failed

        };  // TFlowState

        /* TODO */
        typedef std::function<void (TPov *)> TOnFail;

        /* See TFlowState. */
        TFlowState GetFlowState() const {
          assert(this);
          return FlowState;
        }

        /* The pov which owns us.  Never null. */
        TParentPov *GetParentPov() {
          assert(this);
          return ParentPov;
        }

        /* See TFlowState. */
        void Pause();

        /* See TPov::TryGetParentPov(). */
        virtual TParentPov *TryGetParentPov() const {
          assert(this);
          return ParentPov;
        }

        /* See TFlowState. */
        void Unpause();

        protected:

        /* Attach as a child to the given parent.
           Take the parent's graph root as our own. */
        TChildPov(TParentPov *parent_pov, const TOnFail& on_fail, bool paused=false);

        /* Also detaches from our parent pov. */
        virtual ~TChildPov();

        private:

        /* See TPov::Fail().
           This will cause us to change FlowState to Failed. */
        virtual void Fail();

        /* Join us to our parent's tetris game, if we're not already so joined. */
        void JoinTetris();

        /* Part us from our parent's tetris game, if we're not already so parted. */
        void PartTetris();

        /* See accessor. */
        TFlowState FlowState;

        /* True iff. we're joinined to our parent's tetris game. */
        bool IsJoinedToTetris;


        const TOnFail &OnFail;

        /* See accessor. */
        TParentPov *ParentPov;

        /* The next and previous children of our parent. */
        TChildPov *NextChildPov, *PrevChildPov;

        /* Flowstate. */
        friend class TGlobalPov;

        /* ~TChildPov(). */
        friend class TParentPov;

        /* NextChildPov. */
        friend class TParentPov::TChildPovCursor;

      };  // TChildPov

      /*  ----------------------------------------------------------------------
       *  TGlobalPov.
       */

      /* The foundation of the system.
         The causality graph has exactly one of these. */
      class TGlobalPov
          : public TParentPov {
        NO_COPY_SEMANTICS(TGlobalPov);
        public:

        /* Also constructs the root of the causality graph. */
        TGlobalPov(const std::string &path , bool is_create = true);

        /* Also destroys the root of the causality graph. */
        virtual ~TGlobalPov();

        /* See TPov::TryGetParentPov(). */
        virtual TParentPov *TryGetParentPov() const {
          return 0;
        }

        private:

        /* See TPov::Fail().
           This should never happen here. */
        virtual void Fail();

      };  // TGlobalPov

      /*  ----------------------------------------------------------------------
       *  TPrivatePov.
       */

      /* A final child of the pov tree.  Each one of these represents a separate
         leaf of the causality graph. */
      class TPrivatePov
          : public TChildPov {
        NO_COPY_SEMANTICS(TPrivatePov);
        public:

        /* Also constructs a leaf of the causality graph. */
        TPrivatePov(TParentPov *parent_pov, const TOnFail &on_promote, bool paused);

        /* Also destroys a leaf of the causality graph. */
        virtual ~TPrivatePov();

        /* The leaf of the graph that lives at this terminus of the pov tree.
           Never null. */
        TLeaf *GetLeaf() const {
          assert(this);
          return Leaf;
        }

        private:

        /* See accessor. */
        TLeaf *Leaf;

      };  // TPrivatePov

      /*  ----------------------------------------------------------------------
       *  TSharedPov.
       */

      /* An intermediate level of the pov tree.  We can have zero or more of
         these. */
      class TSharedPov
          : public TParentPov, public TChildPov {
        NO_COPY_SEMANTICS(TSharedPov);
        public:

        /* Do-nothing. */
        TSharedPov(TParentPov *parent_pov, const TOnFail &on_fail, bool paused=false)
            : TChildPov(parent_pov, on_fail, paused) {}

        /* Destructor for a shared pov. We have to call DeleteAllTrailingUpdate
           before the base class destructs. */
        ~TSharedPov();

        private:

      };  // TSharedPov

      /*  ----------------------------------------------------------------------
       *  TEvent.
       */

      /* The base for all events in the causality graph.
         Every event is owned by exactly one pov, but which
         pov owns which event can change over time. */
      class TEvent {
        NO_COPY_SEMANTICS(TEvent);
        public:

        /* The pov which owns this event.  Never null. */
        TPov *GetPov() const {
          assert(this);
          assert(Pov);
          return Pov;
        }

        protected:

        /* Do-nothing. */
        TEvent()
            : Pov(0) {}

        /* Do-nothing. */
        virtual ~TEvent();

        /* See accessor. */
        TPov *Pov;

      };  // TEvent

      /*  ----------------------------------------------------------------------
       *  TCause.
       */

      /* An event which can cause another event.
         The inheritance here is virtual to support the diamond at TUpdate. */
      class TCause
          : public virtual TEvent {
        NO_COPY_SEMANTICS(TCause);
        public:

        /* Iterates forward through the effects of a cause. */
        class TEffectCursor {
          NO_COPY_SEMANTICS(TEffectCursor);
          public:

          /* The type of thing we iterate. */
          typedef TEffect TItem;

          /* Begin at the first effect of the given cause.
             Passing a cause with no effects will yield an exhausted iterator.
             Passing a null cause will also yield an exhausted iterator. */
          explicit TEffectCursor(const TCause *cause)
              : LinkToEffect(cause ? cause->FirstLinkToEffect : 0) {}

          /* Begin at the effect linked to by the given link.
             Passing a null will yield an exhausted iterator. */
          explicit TEffectCursor(TLink *link_to_effect)
              : LinkToEffect(link_to_effect) {}

          /* True iff. we're not yet exhausted. */
          operator bool() const {
            assert(this);
            return LinkToEffect != 0;
          }

          /* A pointer to the current effect.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TEffect *operator*() const;

          /* A pointer to the current effect.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TEffect *operator->() const;

          /* Advance beyond the current effect.
             Calling this function on an exhausted iterator is an error. */
          TEffectCursor &operator++();

          /* The link to the current effect.
             Calling this function on an exhausted iterator is an error. */
          TLink *GetLinkToEffect() const {
            assert(this);
            assert(LinkToEffect);
            return LinkToEffect;
          }

          /* The link to the current effect.
             If the iterator is exhausted, return null. */
          TLink *TryGetLinkToEffect() const {
            assert(this);
            return LinkToEffect;
          }

          private:

          /* See accessor. */
          TLink *LinkToEffect;

        };  // TEffectCursor

        /* True if we could count as a local cause for the given event.
           This does not check to see if we're actually linked to the event. */
        virtual bool IsLocal(const TEvent *event) const = 0;

        /* Return the link which leads from this cause to the given effect.
           If there is no such link, return null. */
        TLink *TryGetLinkToEffect(const TEffect *effect) const;

        protected:

        /* Do-nothing. */
        TCause()
            : FirstLinkToEffect(0), LastLinkToEffect(0) {}

        /* The descendant must delete each link to effects before destruction reaches this point. */
        virtual ~TCause();

        /* Bye-bye, links to effects. */
        void DeleteEachLinkToEffect();

        private:

        /* The first and last links to the effects of this cause. */
        TLink *FirstLinkToEffect, *LastLinkToEffect;

        /* FirstLinkToEffect and LastLinkToEffect. */
        friend class TLink;

      };  // TCause

      /*  ----------------------------------------------------------------------
       *  TEffect.
       */

      /* An event which can be caused by another event.
         The inheritance here is virtual to support the diamond at TUpdate. */
      class TEffect
          : public virtual TEvent {
        NO_COPY_SEMANTICS(TEffect);
        public:

        /* Iterates forward through the causes of a effect. */
        class TCauseCursor {
          NO_COPY_SEMANTICS(TCauseCursor);
          public:

          /* The type of thing we iterate. */
          typedef TCause TItem;

          /* Begin at the first cause of the given effect.
             Passing a effect with no causes will yield an exhausted iterator.
             Passing a null effect will also yield an exhausted iterator. */
          explicit TCauseCursor(const TEffect *effect)
              : LinkToCause(effect ? effect->FirstLinkToCause : 0) {}

          /* Begin at the cause linked to by the given link.
             Passing a null will yield an exhausted iterator. */
          explicit TCauseCursor(TLink *link_to_cause)
              : LinkToCause(link_to_cause) {}

          /* True iff. we're not yet exhausted. */
          operator bool() const {
            assert(this);
            return LinkToCause != 0;
          }

          /* A pointer to the current cause.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TCause *operator*() const;

          /* A pointer to the current cause.  Never null.
             Calling this function on an exhausted iterator is an error. */
          TCause *operator->() const;

          /* Advance beyond the current cause.
             Calling this function on an exhausted iterator is an error. */
          TCauseCursor &operator++();

          /* The link to the current cause.
             Calling this function on an exhausted iterator is an error. */
          TLink *GetLinkToCause() const {
            assert(this);
            assert(LinkToCause);
            return LinkToCause;
          }

          /* The link to the current cause.
             If the iterator is exhausted, return null. */
          TLink *TryGetLinkToCause() const {
            assert(this);
            return LinkToCause;
          }

          private:

          /* See accessor. */
          TLink *LinkToCause;

        };  // TCauseCursor

        /* If this effect is an update, add it to the given collection of effects,
           as well as all updates which follow it in the path toward the leaf of the
           given private pov.  If this effect is a leaf, stop recursing. */
        virtual void AugmentEffectsRecursively(
            std::unordered_set<const TUpdate *> &effects,
            const TPov *pov) const = 0;

        /* True iff. this effect has at no non-root causes in the same pov as it.

           Note: This function checks to see if we're ACTUALLY trailing,
           as opposed to simply checking our link to our pov.  This is
           useful in unit testing but isn't used in production. */
        bool GetIsTrailing() const;

        /* Return the link which leads from this effect to the given cause.
           If there is no such link, return null. */
        TLink *TryGetLinkToCause(const TCause *cause) const;

        protected:

        /* Do-nothing. */
        TEffect()
            : FirstLinkToCause(0), LastLinkToCause(0) {}

        /* The descendant must delete each link to causes before destruction reaches this point. */
        virtual ~TEffect();

        /* Bye-bye, links to causes. */
        void DeleteEachLinkToCause();

        /* Notice that a local cause now links to this effect.
           Called by TLink::SetCause() and TLink::SetEffect(). */
        virtual void OnLocalCauseJoin() = 0;

        /* Notice that a local cause no longer links to this effect.
           Called by TLink::SetCause(), TLink::SetEffect(), and TUpdate::Promote(). */
        virtual void OnLocalCausePart() = 0;

        private:

        /* The first and last links to the causes of this effect. */
        TLink *FirstLinkToCause, *LastLinkToCause;

        /* OnLocalCauseJoin(), OnLocalCausePart(), FirstLinkToCause, and LastLinkToCause. */
        friend class TLink;

        /* OnLocalCausePart(). */
        friend class TUpdate;

      };  // TEffect

      /*  ----------------------------------------------------------------------
       *  TLink.
       */

      /* A link from a cause to an effect.
         NOTE: It's SUPER-IMPORTANT that links NOT FORM CYCLES in the causality graph.
         As we are meant only to create updates at the leaf-end of the graph, this should
         never happen, and we therefore don't check for it.  If we ever get into creating
         updates elsewhere in the graph, we should insert a checks against cycles in the
         SetCause() and SetEffect() functions. */
      class TLink {
        NO_COPY_SEMANTICS(TLink);
        public:

        /* The cause to which we link.  Never null. */
        TCause *GetCause() const {
          assert(this);
          assert(Cause);
          return Cause;
        }

        /* The effect to which we link.  Never null. */
        TEffect *GetEffect() const {
          assert(this);
          assert(Effect);
          return Effect;
        }

        /* True iff. only link between our cause and any effect. */
        bool CauseHasOnlyOneEffect() const {
          assert(this);
          return Cause && !NextLinkToEffect && !PrevLinkToEffect;
        }

        private:

        /* Called only by TUpdate::TUpdate(). */
        TLink(TCause *cause, TEffect *effect)
            : Cause(0), NextLinkToEffect(0), PrevLinkToEffect(0),
              Effect(0), NextLinkToCause(0), PrevLinkToCause(0) {
          assert(cause);
          assert(effect);
          SetCause(cause);
          SetEffect(effect);
        }

        /* Called only by TCause::~TCause() and TEffect::~TEffect(). */
        ~TLink() {
          assert(this);
          SetEffect(0);
          SetCause(0);
        }

        /* Insert this link last in the given cause's list of links to effects.
           Unlink from our current cause (if any) before linking to the new one.
           If the given cause pointer is null, unlink, if necessary, and stay unlinked.
           If the change in linkage results in the joining and/or parting of a local cause, notify the effect's handler(s).
           If the given cause is the same as our current one, do nothing. */
        void SetCause(TCause *new_cause) {
          assert(this);
          if (Cause != new_cause) {
            if (Cause) {
              TCause *old_cause = Cause;
              (PrevLinkToEffect ? PrevLinkToEffect->NextLinkToEffect : Cause->FirstLinkToEffect) = NextLinkToEffect;
              (NextLinkToEffect ? NextLinkToEffect->PrevLinkToEffect : Cause->LastLinkToEffect) = PrevLinkToEffect;
              Cause = 0;
              NextLinkToEffect = 0;
              PrevLinkToEffect = 0;
              if (Effect && old_cause->IsLocal(Effect)) {
                Effect->OnLocalCausePart();
              }
            }
            if (new_cause) {
              assert(!NextLinkToEffect);
              Cause = new_cause;
              PrevLinkToEffect = Cause->LastLinkToEffect;
              (PrevLinkToEffect ? PrevLinkToEffect->NextLinkToEffect : Cause->FirstLinkToEffect) = this;
              Cause->LastLinkToEffect = this;
              if (Effect && new_cause->IsLocal(Effect)) {
                Effect->OnLocalCauseJoin();
              }
            }
          }
        }

        /* Insert this link last in the given effect's list of links to causes.
           Unlink from our current effect (if any) before linking to the new one.
           If the given effect pointer is null, unlink, if necessary, and stay unlinked.
           If the change in linkage results in the joining and/or parting of a local cause, notify the effect's handler(s).
           If the given effect is the same as our current one, do nothing. */
        void SetEffect(TEffect *new_effect) {
          assert(this);
          if (Effect != new_effect) {
            if (Effect) {
              TEffect *old_effect = Effect;
              (PrevLinkToCause ? PrevLinkToCause->NextLinkToCause : Effect->FirstLinkToCause) = NextLinkToCause;
              (NextLinkToCause ? NextLinkToCause->PrevLinkToCause : Effect->LastLinkToCause) = PrevLinkToCause;
              Effect = 0;
              NextLinkToCause = 0;
              PrevLinkToCause = 0;
              if (Cause && Cause->IsLocal(old_effect)) {
                old_effect->OnLocalCausePart();
              }
            }
            if (new_effect) {
              assert(!NextLinkToCause);
              Effect = new_effect;
              PrevLinkToCause = Effect->LastLinkToCause;
              (PrevLinkToCause ? PrevLinkToCause->NextLinkToCause : Effect->FirstLinkToCause) = this;
              Effect->LastLinkToCause = this;
              if (Cause && Cause->IsLocal(new_effect)) {
                new_effect->OnLocalCauseJoin();
              }
            }
          }
        }

        /* See accessor. */
        TCause *Cause;

        /* The next and previous links to effects of our cause. */
        TLink *NextLinkToEffect, *PrevLinkToEffect;

        /* See accessor. */
        TEffect *Effect;

        /* The next and previous links to causes of our effect. */
        TLink *NextLinkToCause, *PrevLinkToCause;

        /* ~TLink(). */
        friend class TCause;

        /* Effect and NextLinkToEffect. */
        friend class TCause::TEffectCursor;

        /* ~TLink() */
        friend class TEffect;

        /* Cause and NextLinkToCause. */
        friend class TEffect::TCauseCursor;

        /* TLink() and SetEffect(). */
        friend class TUpdate;

      };  // TLink

      /*  ----------------------------------------------------------------------
       *  TRoot.
       */

      /* The root cause of all events in a causality graph.
         There is exactly one instance of this class in the whole graph, and it's owned by TGlobalPov. */
      class TRoot
          : public TCause {
        NO_COPY_SEMANTICS(TRoot);
        public:

        /* See TCause::IsLocal().
           The root never counts as a local cause. */
        virtual bool IsLocal(const TEvent */*event*/) const {
          return false;
        }

        private:

        /* Do-nothing. */
        TRoot(TPov *pov) {
          assert(pov);
          Pov = pov;
        }

        /* Do-nothing. */
        virtual ~TRoot();

        /* TRoot() and ~TRoot(). */
        friend class TGlobalPov;

      };  // TRoot

      /*  ----------------------------------------------------------------------
       *  TLeaf.
       */

      /* The leaf effect of some set of events in a causality graph.
         There is exactly one instance of this class per private point-of-view. */
      class TLeaf
          : public TEffect {
        NO_COPY_SEMANTICS(TLeaf);
        public:

        /* See TEffect::AugmentEffectsRecursively(). */
        virtual void AugmentEffectsRecursively(
            std::unordered_set<const TUpdate *> &effects,
            const TPov *pov) const;

        private:

        /* Do-nothing. */
        TLeaf(TPov *pov) {
          assert(pov);
          Pov = pov;
        }

        /* Do-nothing. */
        virtual ~TLeaf();

        private:

        /* See TEffect::OnCauseJoin().
           For us, this is do-nothing. */
        virtual void OnLocalCauseJoin();

        /* See TEffect::OnCauseJoin().
           For us, this is do-nothing. */
        virtual void OnLocalCausePart();

        /* TLeaf() and ~TLeaf(). */
        friend class TPrivatePov;

      };  // TLeaf

      /*  ----------------------------------------------------------------------
       *  TUpdate.
       */

      /* An event which represents a change in state in the database.
         This will be caused by earlier events (ultimately the root of the global pov),
         and will also be the cause of later events (ultimately the leaf of one or more private psov). */
      class TUpdate
          : public TCause, public TEffect, public TTetrisPiece {
        NO_COPY_SEMANTICS(TUpdate);
        public:

        typedef std::function<void (const TPov *pov)> TOnPromote;

        /* The new update will be linked into the leaf end of the given private pov,
           with the given updates as its causes.  The set of causes can be empty.
           If the update has no causes in the private pov in which it is created, it
           will also be linked in as a tailing update and be therefore a candidate for promotion.
           The update will construct a node cluster iff. the given set of node values is not empty.
           The update will construct an edge cluster iff. the given set of edge values is not empty. */
        TUpdate(
            TPrivatePov *private_pov,
            const std::unordered_set<TUpdate *> &causes,
            std::unordered_map<TKV, Var::TPtr<Var::TChange>> &&op_by_kv,
            const TAssertChecker &assert_checker,
            size_t max_age = 10,
            TOnPromote &&on_promote=[](const TPov*){}
          );

        /* See TCause::IsLocal().
           We count as a local cause if we're in the same pov as the given event. */
        virtual bool IsLocal(const TEvent *event) const {
          assert(this);
          assert(event);
          return Pov == event->GetPov();
        }

        /* Check the assertions associated with this Tetris piece. */
        virtual bool Assert(TContext &ctxt) const;

        /* See TEffect::AugmentEffectsRecursively(). */
        virtual void AugmentEffectsRecursively(
            std::unordered_set<const TUpdate *> &effects,
            const TPov *pov) const;

        /* Mark this update's pov as permanently failed.  The pov will no longer be part of tetris.
           NOTE: Do NOT call this on any update which is in global.  You will abort the program. */
        virtual void Fail() {
          assert(this);
          if(IsLinkedToPov) {
            Pov->Fail();
          }
        }

        /* Enumerate the keys involved in this Tetris piece. */
        virtual bool ForEachKey(const std::function<bool (const Var::TVar &)> &cb) const;

        /* The number of keys involved in this Tetris piece. */
        virtual size_t GetKeyCount() const;

        /* TODO */
        const TAssertChecker &GetAssertChecker() const {
          assert(this);
          assert(AssertChecker); // Make sure the function is valid.

          return AssertChecker;
        }

        /* Move this update from its current pov to the parent of its current pov.
           This is only legal if the update is trailing.
           If the update continues to exist, albeit in the new pov, we return true.
           If the update's current pov has no parent, then the update is simply deleted.  In this case we return false. */
        virtual bool Promote();

        /* The version we keep for the given key, if any. */
        template <typename TKey>
        TVersion<TKey> *TryGetVersion(const TKey &key) const {
          assert(this);
          TCluster<TKey> *cluster;
          GetCluster(cluster);
          return cluster->TryGetVersion(key);
        }

        private:

        /* It is only valid to delete an update which is trailing.
           Doing so will unlink it from its pov and signal its effects that they may now be trailing. */
        virtual ~TUpdate();

        /* The cluster of KV versions for this update.  Null if this update does not update any KVs.
           We return by out-parameter in order to take advantage of function overloading in the template-based classes. */
        void GetCluster(TCluster<TKV> *&out) const {
          assert(this);
          assert(&out);
          out = KVCluster;
        }

        /* If we're not currently linked to our pov as a trailing update, do so now.
           LocalCauseCount must be zero before linking to the pov. */
        void LinkToPov();

        /* See TEffect::OnCauseJoin().
           Increment LocalCauseCount.  If we're currently linked to our pov as a trailing update, unlink. */
        virtual void OnLocalCauseJoin();

        /* See TEffect::OnCauseJoin().
           Increment LocalCauseCount.  If we have no local causes left, link to our pov as a trailing update. */
        virtual void OnLocalCausePart();

        /* If LocalCauseCount is zero, make sure we're linked with our pov.
           If LocalCauseCount is non-zero, make sure we're not linked with our pov. */
        void SyncWithPov();

        /* If we're linked to our pov as a trailing update, unlink now.
           LocalCauseCount must be non-zero before unlinking from the pov. */
        void UnlinkFromPov();

        /* The number of causes of this update which are in the same pov as this update. */
        size_t LocalCauseCount;

        /* True iff. we're currently linked to our pov as a trailing update.*/
        bool IsLinkedToPov;

        /* The next and previous trailing updates in our pov.
           If we're not currently linked, these are null.
           If we are currently linked, either or noth of these could still be null. */
        TUpdate *NextTrailingUpdate, *PrevTrailingUpdate;

        /* The cluster of KVs being versioned by this update.
           If this update makes no reference to KVs, this will be null. */
        TCluster<TKV> *KVCluster;

        /* TODO */
        TAssertChecker AssertChecker;

        /* TODO */
        TOnPromote OnPromote;

        /* TODO */
        friend class TGlobalPov;

        /* NextTrailingUpdate. */
        friend class TPov::TTrailingUpdateCursor;

        /* ~TUpdate(). */
        friend class TPov;

        /* GetCluster<>(). */
        friend class TContext;

      };  // TUpdate

      /*  ----------------------------------------------------------------------
       *  TIndex<TKey>.
       */

      /* Keeps a set of all the keys in a pov and allows you to look them up or iterate through them.
         For each key, there will be at most one tour, which will show you the history of that key's
         updates within the pov.

         This is a base class.  Key-type-specific descendants allow you to search through the keys in
         key-type-specific ways. */
      template <typename TKey>
      class TIndex {
        NO_COPY_SEMANTICS(TIndex);
        public:

        /* A mapping of keys to their tours. */
        typedef std::unordered_map<TKey, TTour<TKey> *> TTourByKeyMap;

        /* True iff. the key exists in the index. */
        bool Exists(const TKey &key) const {
          assert(this);
          return TourByKey.find(key) != TourByKey.end();
        }

        /* Iterate through all the tours in the pov, in no particular order.
           Return false from your callback to halt the iterator.
           If the iterator reaches the end, return true; if halted, return false. */
        bool ForEachTour(const std::function<bool (TTour<TKey> *)> &callback) const {
          assert(this);
          assert(&callback);
          bool result = true;
          for (auto iter = TourByKey.begin(); result && iter != TourByKey.end(); ++iter) {
            result = callback(iter->second);
          }
          return result;
        }

        /* The pov to which we belong. */
        TPov *GetPov() const {
          assert(this);
          return Pov;
        }

        /* The tour associated with the given key, if any. */
        TTour<TKey> *TryGetTour(const TKey &key) const {
          assert(this);
          return Base::FindOrDefault(TourByKey, key, 0);
        }

        protected:

        /* Do-nothing. */
        TIndex(TPov *pov)
            : Pov(pov) {
          assert(pov);
        }

        /* You must make sure that all the slots have been destroyed before destroying the index. */
        virtual ~TIndex() {
          assert(this);
          assert(TourByKey.empty());
        }

        /* Notifies our descendant class that a new tour has joined the pov. */
        virtual void OnTourJoin(const std::pair<TKey, TTour<TKey> *> &kt_pair) = 0;

        /* Notifies our descendant class that an existing tour is parting from the pov. */
        virtual void OnTourPart(const TKey &key) = 0;

        /* Called by a tour to let us know it's joining to us. */
        void RaiseTourJoin(TTour<TKey> *tour) {
          assert(this);
          assert(tour);
          OnTourJoin(std::pair<TKey, TTour<TKey> *>(tour->GetKey(), tour));
        }

        /* Called by a tour to let us know it's parting from us. */
        void RaiseTourPart(TTour<TKey> *tour) {
          assert(this);
          assert(tour);
          const TKey &key = tour->GetKey();
          TourByKey.erase(key);
          OnTourPart(key);
        }

        protected:

        /* The tour associated with the given key.  If there is no such tour, create one. */
        TTour<TKey> *GetTour(const TKey &key) {
          assert(this);
          /* Try to find the tour for the given key.  If it's not already in our
             map, insert a null to hold its place. */
          auto result = TourByKey.insert(std::pair<TKey, TTour<TKey> *>(key, 0));
          /* We found a null (either because we just inserted it or because it was
             inserted before but the constructor for the tour failed), so try to construct
             a new tour. */
          if (!result.first->second) {
            result.first->second = new TTour<TKey>(this, key);
          }
          /* By now, we have it. */
          assert(result.first->second);
          return result.first->second;
        }

        /* See accessor. */
        TPov *Pov;

        /* All our tours. */
        TTourByKeyMap TourByKey;

        /* RaiseTourJoin and RaiseTourPart. */
        friend class TTour<TKey>;

        /* GetTour(). */
        friend class TVersion<TKey>;

      };  // TIndex<TKey>

      /*  ----------------------------------------------------------------------
       *  TNodeIndex.
       */

      /* The KV-specific descendant of TIndex<>. */
      class TKVIndex
          : public TIndex<TKV> {
        NO_COPY_SEMANTICS(TKVIndex);
        public:

        /* A mapping of KVs to their tours using the K comparator. */
        typedef std::map<TKV, TTour<TKV> *, TKV::TkComparator> TkMap;

        typedef std::unordered_set<TKV> TTabooSet;

        /* TODO */
        class TKeyIndexCursor {
          NO_COPY_SEMANTICS(TKeyIndexCursor);
          public:

          /* TODO */
          typedef TKV TItem;

          /* TODO */
          TKeyIndexCursor(const TKVIndex *index, const Var::TVar &pattern, const Base::TUuid &index_id);

          /* TODO */
          ~TKeyIndexCursor();

          /* True iff. we're not yet exhausted. */
          operator bool() const {
            assert(this);
            if (!Valid) {
              GetValue();
            }
            return KV != 0;
          }

          /* A reference to the current KV.
             Calling this function on an exhausted iterator is an error. */
          const TKV &operator*() const {
            assert(this);
            if (!Valid) {
              GetValue();
            }
            assert(KV);
            return *KV;
          }

          /* A pointer to the current KV.  Never null.
             Calling this function on an exhausted iterator is an error. */
          const TKV *operator->() const {
            assert(this);
            if (!Valid) {
              GetValue();
            }
            assert(KV);
            return KV;
          }

          /* TODO */
          TKeyIndexCursor &operator++();

          private:

          /* TODO */
          void GetValue() const;

          /* TODO */
          mutable bool Valid;

           /* TODO */
          /* I'm pretty sure it should look more like : const TKV * mutable KV; but that doesn't seem to be syntactically correct?
             The pointer is mutable, the value is not. */
          mutable const TKV * KV;

          /* TODO */
          mutable TPov *Pov;

          /* TODO */
          mutable std::vector<std::pair<TTourByKeyMap::const_iterator, TTourByKeyMap::const_iterator>> TourByKeyIterVec;

          /* TODO */
          mutable std::unordered_set<size_t> TourByKeyIterStateSet;

          /* TODO */
          mutable std::vector<std::pair<TkMap::const_iterator, TkMap::const_iterator>> kIterVec;

          /* TODO */
          mutable std::unordered_set<size_t> kIterStateSet;

          /* TODO */
          std::vector<TSync::TSharedLock *> SharedLockVec;

          /* TODO */
          Var::TVar Pattern;

          /* TODO */
          Base::TUuid IndexId;

        };  // TKeyIndexCursor

        #if 0
        /* Iterate, in no particular order, all the tours in the pov which match the given criteria.
           Passing a null for a criterian makes it a wildcard.
           Return false from your callback to halt the iterator.
           If the iterator reaches the end, return true; if halted, return false. */
        bool ForEachTour(
            const std::function<bool (TTour<TNode> *)> &callback,
            const std::string *location, const std::string *klass) const;
        #endif

        protected:

        /* Do-nothing. */
        TKVIndex(TPov *pov) : TIndex<TKV>(pov) {}

        /* Do-nothing, but see TIndex<>::~TIndex() for information about an important restriction. */
        virtual ~TKVIndex();

        /* See TIndex<>::OnTourJoin().
           We'll use this handler to update our in-order maps. */
        virtual void OnTourJoin(const std::pair<TKV, TTour<TKV> *> &kt_pair);

        /* See TIndex<>::OnTourPart().
           We'll use this handler to update our in-order maps. */
        virtual void OnTourPart(const TKV &key);

        /* Our in-order maps. */
        TkMap K;

        /* TKVIndex() and ~TKVIndex(). */
        friend class TPov;

      };  // TKVIndex

      /*  ----------------------------------------------------------------------
       *  TTour<TKey>.
       */

      /* The set of updates within a pov which pertain to a particular key.
         This constitutes a 'tour' of the updates in the causality graph, hence the name.
         The tour is in chronological order, relative to the pov. */
      template <typename TKey>
      class TTour {
        NO_COPY_SEMANTICS(TTour);
        public:

        /* The index of our key type within the pov.  Never null. */
        TIndex<TKey> *GetIndex() const {
          assert(this);
          return Index;
        }

        /* The key we represent. */
        const TKey &GetKey() const {
          assert(this);
          return Key;
        }

        /* The newest version in our tour */
        TVersion<TKey> *GetNewestVersion() const {
          assert(this);
          return NewestVersion;
        }

        /* The oldest version in our tour */
        TVersion<TKey> *GetOldestVersion() const {
          assert(this);
          return OldestVersion;
        }

        /* The tour for our key belonging to an ancestor of our pov.
           We'll return the tour nearest to us, scanning upward in the pov tree.
           If there is no ancestor pov containing a tour for our key, return null. */
        TTour<TKey> *TryGetAncestorTour(std::vector<TSync::TSharedLock *> &shared_lock_vec) const {
          assert(this);
          TTour<TKey> *result = 0;
          const TPov *pov = Index->GetPov();
          do {
            pov = pov->TryGetParentPov();
            if (!pov) {
              break;
            }
            TIndex<TKey> *index;
            shared_lock_vec.push_back(new TSync::TSharedLock(pov->GetSync()));
            pov->GetIndex(index);
            result = index->TryGetTour(Key);
          } while (!result);
          return result;
        }

        /* The tour for our key belonging to a descendant of our pov w.r.t. the given private pov.
           We'll return the tour nearest to us, scanning downward in the pov tree.
           If there is no descendant containing a tour for our key, return null. */
        TTour<TKey> *TryGetDescendantTour(const TPrivatePov *private_pov) const {
          assert(this);
          TTour<TKey> *result = 0;
          const TPov *pov = Index->GetPov();
          do {
            pov = pov->TryGetChildPov(private_pov);
            if (!pov) {
              break;
            }
            TIndex<TKey> *index;
            pov->GetIndex(index);
            result = index->TryGetTour(Key);
          } while (!result);
          return result;
        }

        /* Search for the tour for the given key which is at or above the given pov. */
        static TTour<TKey> *TryGetTour(const TPov *pov, const TKey &key, std::vector<TSync::TSharedLock *> &shared_lock_vec) {
          TTour<TKey> *tour = 0;
          for (; pov; pov = pov->TryGetParentPov()) {
            TIndex<TKey> *index;
            shared_lock_vec.push_back(new TSync::TSharedLock(pov->GetSync()));
            pov->GetIndex(index);
            tour = index->TryGetTour(key);
            if (tour) {
              break;
            }
          }
          return tour;
        }

        private:

        /* We start with no versions of the key's value, but we expect one to arrive immediately after we
           construct.  A tour with no versions is not in a good state. */
        TTour(TIndex<TKey> *index, const TKey &key)
            : Index(index), Key(key), OldestVersion(0), NewestVersion(0) {
          assert(index);
          /* We let our index know we've joined. */
          index->RaiseTourJoin(this);
        }

        /* Do not attempt to destroy a tour while it still has versions.  Destroy the versions and let
           the last of them destroy the tour for you. */
        ~TTour() {
          assert(this);
          assert(!OldestVersion && !NewestVersion);
          /* We let our index know we're parting. */
          assert(Index);
          Index->RaiseTourPart(this);
        }

        /* See accessor. */
        TIndex<TKey> *Index;

        /* See accessor. */
        TKey Key;

        /* The oldest and newest versions.  These will be null immediately after construction, but,
           after the first version constructs, these should remain non-null. */
        TVersion<TKey> *OldestVersion, *NewestVersion;

        /* TTour(). */
        friend class TIndex<TKey>;

        /* ~TTour(), OldestVersion, and NewestVersion. */
        friend class TVersion<TKey>;

        /* NewestVersion. */
        friend class TContext;

      };  // TTour<TKey>

      /*  ----------------------------------------------------------------------
       *  TVersion<TKey>.
       */

      /* A change of state for a key within a pov.
         This is an associative entity, joining a cluster to a tour. */
      template <typename TKey>
      class TVersion {
        NO_COPY_SEMANTICS(TVersion);
        public:

        /* Each type of key as associated with it a kind of operation which
           can be performed on that key.  Each version stores a single such operation.
           For convenience, we define the type of the op into scope here. */
        typedef typename TKey::TOp TOp;

        /* The cluster of which we are a part.  Never null. */
        TCluster<TKey> *GetCluster() const {
          assert(this);
          return Cluster;
        }

        /* The operation we perform on the key's value. */
        const TOp &GetOp() const {
          assert(this);
          return Op;
        }

        /* The tour of which we are a part.  Never null. */
        TTour<TKey> *GetTour() const {
          assert(this);
          return Tour;
        }

        /* The version which follows us in our key's timeline.  This version may belong to a pov from which descends from our pov,
           w.r.t. the given private pov.  If we are the newest version for this key, return null. */
        TVersion<TKey> *TryGetAnyNewerVersion(const TPrivatePov *private_pov) const {
          assert(this);
          TVersion<TKey> *result = NewerVersion;
          if (!result) {
            TTour<TKey> *tour = Tour->TryGetDescendantTour(private_pov);
            if (tour) {
              result = tour->OldestVersion;
            }
          }
          return result;
        }

        /* The version which precedes us in our key's timeline.  This version may belong to a pov from which our pov descends.
           If we are the oldest version for this key in the whole pov tree, return null. */
        TVersion<TKey> *TryGetAnyOlderVersion(std::vector<TSync::TSharedLock *> &shared_lock_vec) const {
          assert(this);
          TVersion<TKey> *result = OlderVersion;
          if (!result) {
            TTour<TKey> *tour = Tour->TryGetAncestorTour(shared_lock_vec);
            if (tour) {
              result = tour->NewestVersion;
            }
          }
          return result;
        }

        private:

        /* Connects the given cluster to the tour of the given key.
           We'll become the newest version in the tour.
           If we are the first version of this key in this pov, we'll construct the tour. */
        TVersion(TCluster<TKey> *cluster, const TKey &key, const TOp &op)
            : Op(op), Cluster(cluster), Tour(0), OlderVersion(0), NewerVersion(0) {
          assert(cluster);
          LinkToTour(key);
          Base::InsertOrFail(cluster->VersionByKey, std::pair<TKey, TVersion<TKey> *>(key, this));
        }

        /* Unlink from our cluster and from our tour.
           If we're the only version in the tour, we'll destroy the tour as well. */
        ~TVersion() {
          assert(this);
          Cluster->VersionByKey.erase(Tour->GetKey());
          UnlinkFromTour();
        }

        /* Unlink from our current tour, if any, and link to the tour for the given key within our
           cluster's index.  If no such tour yet exists, create it. */
        void LinkToTour(const TKey &key) {
          assert(this);
          TTour<TKey> *tour = Cluster->GetIndex()->GetTour(key);
          UnlinkFromTour();
          assert(!NewerVersion);
          OlderVersion = tour->NewestVersion;
          (OlderVersion ? OlderVersion->NewerVersion : tour->OldestVersion) = this;
          tour->NewestVersion = this;
          Tour = tour;
        }

        /* If we're currently linked to a tour, unlink.
           If we were the only version in the tour, destroy the tour. */
        void UnlinkFromTour() {
          assert(this);
          if (Tour) {
            (OlderVersion ? OlderVersion->NewerVersion : Tour->OldestVersion) = NewerVersion;
            (NewerVersion ? NewerVersion->OlderVersion : Tour->NewestVersion) = OlderVersion;
            if (!Tour->OldestVersion) {
              assert(!Tour->NewestVersion);
              delete Tour;
            }
            Tour = 0;
            NewerVersion = 0;
            OlderVersion = 0;
          }
        }

        /* See accessor. */
        TOp Op;

        /* See accessor. */
        TCluster<TKey> *Cluster;

        /* See accessor. */
        TTour<TKey> *Tour;

        /* The next-oldest and -newest versions in our tour.
           Either of both of these could be null. */
        TVersion *OlderVersion, *NewerVersion;

        /* TVersion, ~TVersion, and LinkToTour(). */
        friend class TCluster<TKey>;

      };  // TVersion<TKey>

      /*  ----------------------------------------------------------------------
       *  TCluster<TKey>.
       */

      /* A cluster of simultaneous changes to keys of the same type.
         We will belong to an update in which clusters of updates to other key types may also reside. */
      template <typename TKey>
      class TCluster {
        NO_COPY_SEMANTICS(TCluster);
        public:

        /* See TVersion<TKey>::TOp. */
        typedef typename TKey::TOp TOp;

        /* Add to the given set of causes any updates which are implied by our set of versions. */
        void AugmentCauses(std::unordered_set<TUpdate *> &causes, std::vector<TSync::TSharedLock *> &shared_lock_vec) const {
          assert(this);
          assert(&causes);
          for (auto iter = VersionByKey.begin(); iter != VersionByKey.end(); ++iter) {
            TVersion<TKey> *older_version = iter->second->TryGetAnyOlderVersion(shared_lock_vec);
            if (older_version) {
              causes.insert(older_version->Cluster->GetUpdate());
            }
          }
        }

        /* The index which keeps our keys.  Never null. */
        TIndex<TKey> *GetIndex() const {
          assert(this);
          TIndex<TKey> *index;
          Update->GetPov()->GetIndex(index);
          assert(index);
          return index;
        }

        /* The update to which we blong.  Never null. */
        TUpdate *GetUpdate() const {
          assert(this);
          return Update;
        }

        /* TODO */
        const std::unordered_map<TKey, TVersion<TKey> *> &GetVersionByKey() const {
          assert(this);
          return VersionByKey;
        }

        /* The version we keep for the given key, if any. */
        TVersion<TKey> *TryGetVersion(const TKey &key) const {
          assert(this);
          return Base::FindOrDefault(VersionByKey, key, 0);
        }

        private:

        /* Connect to the given update and construct versions for all the given key-op pairs. */
        TCluster(
            TUpdate *update,
            const std::unordered_map<TKey, TOp> &op_by_key)
            : Update(update) {
          assert(update);
          try {
            for (auto iter = op_by_key.begin(); iter != op_by_key.end(); ++iter) {
              new TVersion<TKey>(this, iter->first, iter->second);
            }
          } catch (...) {
            DeleteEachVersion();
            throw;
          }
        }

        /* We delete our versions when we go. */
        ~TCluster() {
          assert(this);
          DeleteEachVersion();
        }

        /* Delete each of our versions.
           We use this to clean up during ctor failure and dtor. */
        void DeleteEachVersion() {
          assert(this);
          /* TODO (jlucas): This isn't a particularly time-efficient way to clear a map. */
          while (!VersionByKey.empty()) {
            delete VersionByKey.begin()->second;
          }
        }

        /* Called by TUpdate::Promote() after the update has promoted to a parent pov.
           We repeat the message to our versions to allow them to chance tours. */
        void OnPromote() {
          assert(this);
          try {
            for (auto iter = VersionByKey.begin(); iter != VersionByKey.end(); ++iter) {
              iter->second->LinkToTour(iter->first);
            }
          } catch (...) {
            /* TODO (jlucas): Under low memory conditions, this could fail, leaving us in a half-changed
               state.  We need to repair this by (1) pre-constructing all the necessary tours in the new
               pov and (2) switching to invasive containment for the unordered maps.  For now, we will
               simply detect the badness and abort the process entirely. */
            Base::TError::Abort(HERE);
          }
        }

        /* See accessor. */
        TUpdate *Update;

        /* All our versions. */
        std::unordered_map<TKey, TVersion<TKey> *> VersionByKey;

        /* TCluster(), ~TCluster(), and OnPromote(). */
        friend class TUpdate;

        /* VersionByKey. */
        friend class TVersion<TKey>;

      };  // TCluster<TKey>

      /* A cursor over a flux capacitor, providing a consistent, time-traveling, context
         in which to evaluate expressions.  This is a value type. */
      class TContext
          : public TContextBase {
        NO_COPY_SEMANTICS(TContext);
        public:

        /* TODO */
        class TKeyCursor
            : public Stig::TKeyCursor {
          NO_COPY_SEMANTICS(TKeyCursor);
          public:

          /* TODO */
          TKeyCursor(TContext *context, Atom::TCore::TExtensibleArena *arena, const Var::TVar &pattern, const Base::TUuid &index_id)
              : Stig::TKeyCursor(arena),
                Cached(false),
                Context(context),
                IndexId(index_id),
                Cursor(Context->Pov->GetKVIndex(), pattern, index_id) {}

          /* TODO */
          virtual ~TKeyCursor() {}

          /* TODO */
          virtual operator bool() const {
            assert(this);
            Refresh();
            return Cursor;
          }

          /* TODO */
          virtual const Indy::TKey &operator*() const {
            assert(this);
            Refresh();
            assert(Cursor);
            return Key;
          }

          /* TODO */
          virtual const Indy::TKey *operator->() const {
            assert(this);
            Refresh();
            assert(Cursor);
            return &Key;
          }

          /* TODO */
          virtual TKeyCursor &operator++() {
            assert(this);
            Cached = false;
            ++Cursor;
            Refresh();
            return *this;
          }

          private:

          /* Advance the cursor to a valid state. */
          void Refresh() const {
            assert(this);
            if (!Cached) {
              Cached = true;
              void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
              while (Cursor && !Context->Exists(Indy::TIndexKey(IndexId, Indy::TKey(Atom::TCore(Arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, Cursor->GetKey()))), Arena)))) {
                ++Cursor;
              }
              if (Cursor) {
                Key = Indy::TKey(Atom::TCore(Arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, Cursor->GetKey()))), Arena);
              }
            }
          }

          /* TODO */
          mutable bool Cached;

          /* TODO */
          mutable TContext *Context;

          /* TODO */
          const Base::TUuid IndexId;

          /* TODO */
          mutable TKVIndex::TKeyIndexCursor Cursor;

          /* TODO */
          mutable Indy::TKey Key;

        };  // TKeyCursor

        /* Begin at the leaf of the given point-of-view. */
        explicit TContext(TPov *pov, Atom::TCore::TExtensibleArena *arena)
            : TContextBase(arena), Pov(pov) {
          assert(pov);
        }

        #if 0
        /* Move-construct.  The donor object resets to the leaf of its point-of-view. */
        TContext(TContext &&that)
            : Pov(that.Pov) {
          assert(&that);
          std::swap(InvalidUpdates, that.InvalidUpdates);
          std::swap(VersionByKV, that.VersionByKV);
          std::swap(SharedLockVec, that.SharedLockVec);
        }
        #endif

        #if 0
        /* Copy constructor.
           Note: This is the same as the compiler-provided one, but, for some reason, the c-
           wrapper can't see the compiler-provided one.  So we'll be explicit in the face of
           ineffable mystery. */
        TContext(const TContext &that)
            : Pov(that.Pov), InvalidUpdates(that.InvalidUpdates), VersionByKV(that.VersionByKV) {
          try {
            for (auto iter = that.SharedLockVec.begin(); iter != that.SharedLockVec.end(); ++iter) {
              SharedLockVec.push_back(new TSync::TSharedLock((*iter)->GetSync()));
            }
          } catch (...) {
            for (auto iter = SharedLockVec.rbegin(); iter != SharedLockVec.rend(); ++iter) {
              delete *iter;
            }
            throw;
          }
        }
        #endif

        /* TODO */
        virtual ~TContext() {
          assert(this);
          for (auto iter = SharedLockVec.rbegin(); iter != SharedLockVec.rend(); ++iter) {
            delete *iter;
          }
        }

        #if 0
        /* Swaperator. */
        TContext &operator=(TContext &&that) {
          assert(this);
          assert(&that);
          std::swap(Pov, that.Pov);
          std::swap(InvalidUpdates, that.InvalidUpdates);
          std::swap(VersionByKV, that.VersionByKV);
          std::swap(SharedLockVec, that.SharedLockVec);
          return *this;
        }
        #endif

        #if 0
        /* Assignment operator.
           Note: See the copy constructor for a brief discussion of why this is here. */
        TContext &operator=(const TContext &that) {
          assert(this);
          return *this = TContext(that);
        }
        #endif

        #if 0
        /* TODO */
        template <typename TKey>
        Var::TVar operator[](const TKey &key) /* const */ { /* making this non-const so that the interpreter can backstep this specific instance of the context: saves on new locks */
          assert(this);
          Var::TVar ret;
          /* TODO: this is where we're going to run the "interpreter" to apply all non-assignment operations to get a final value to return */
          TVersion<TKey> *version = TryGetVersion(key);
          if (version) {
            const typename TKey::TOp op = version->GetOp();
            op.Call(ret, &key, this);
          }
          return ret;
        }
        #endif

        /* TODO */
        #if 0
        template <typename TKey>
        #endif
        virtual Indy::TKey operator[](const Indy::TIndexKey &index_key) override {
          assert(this);
          const Indy::TKey &key = index_key.GetKey(); /* TODO: we should use the index information properly. */
          Var::TVar ret;
          Atom::TCore core;
          std::vector<TVersion<TKV> *> version_vec;
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          TVersion<TKV> *version = TryGetVersion(TKV(Var::ToVar(*Sabot::State::TAny::TWrapper(key.GetState(state_alloc))), index_key.GetIndexId()));
          while (version) {
            version_vec.push_back(version);
            if (version->GetOp()->IsFinal()) {
              break;
            }
            version = version->TryGetAnyOlderVersion(SharedLockVec);
          }
          while (version_vec.size()) {
            const TKV::TOp op = (*version_vec.rbegin())->GetOp();
            op->Apply(ret);
            version_vec.erase(version_vec.end() - 1);
          }
          if (ret) {
            core = Atom::TCore(Arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, ret)).get());
          }
          /*
          Indy::TKey ret_key(Atom::TCore(Arena, Sabot::State::TAny::TWrapper(Var::NewSabot(state_alloc, ret)).get()), Arena);
          std::cout << "Ctx[" << key << "] = [" << ret_key << "]" << std::endl;
          */
          return Indy::TKey(core, Arena);
        }

        /* TODO */
        #if 0
        template <typename TKey>
        #endif
        virtual bool Exists(const Indy::TIndexKey &index_key) override {
          assert(this);
          const Indy::TKey &key = index_key.GetKey(); /* TODO: we should use the index information properly. */
          void *state_alloc = alloca(Sabot::State::GetMaxStateSize());
          TVersion<TKV> *version = TryGetVersion(TKV(Var::ToVar(*Sabot::State::TAny::TWrapper(key.GetState(state_alloc))), index_key.GetIndexId()));
          return version && !version->GetOp()->IsDelete();
        }

        #if 0
        /* Backstep this context away from the given keys. */
        TContext &Backstep(
            const std::unordered_set<TKV> &kvs) {
          return *this = TContext(*this, kvs);
        }

        /* Return a new context which has backstepped away from the given keys. */
        TContext Backstepped(
            const std::unordered_set<TKV> &kvs) {
          return TContext(*this, kvs);
        }
        #endif

        /* Return the version for the given kv, if any. */
        TVersion<TKV> *TryGetVersion(const TKV &key) const {
          assert(this);
          return TryGetVersion(VersionByKV, key);
        }

        private:

        #if 0
        /* Construct a new context, equal to the given one, but backstepped away from the given keys. */
        TContext(
            const TContext &that,
            const std::unordered_set<TKV> &kvs)
            : Pov(that.Pov), InvalidUpdates(that.InvalidUpdates) {
          /* We intentionally do not copy the cached versions from the other context.
             This is because any of them might be invalid due to the backstepping we're about to do. */
          try {
            for (auto iter = that.SharedLockVec.begin(); iter != that.SharedLockVec.end(); ++iter) {
              SharedLockVec.push_back(new TSync::TSharedLock((*iter)->GetSync()));
            }
          } catch (...) {
            for (auto iter = SharedLockVec.rbegin(); iter != SharedLockVec.rend(); ++iter) {
              delete *iter;
            }
            throw;
          }
          AugmentInvalidUpdates(that, kvs);
        }

        /* A callback function for InvalidateDiskKeys that appends keys to the given container. */
        template <typename TKey>
        static bool AppendKeys(const TKey &key, std::unordered_set<TKey> &key_set) {
          key_set.insert(key);
          return true;
        }
        #endif


        /* Invalidate the updates responsible for the current values of the given keys in the given context. */
        template <typename TKey>
        void AugmentInvalidUpdates(const TContext &that, const std::unordered_set<TKey> &keys) {
          assert(this);
          assert(&keys);
          for (auto iter = keys.begin(); iter != keys.end(); ++iter) {
            TVersion<TKey> *version = that.TryGetVersion(*iter);
            if (version) {
              version->GetCluster()->GetUpdate()->AugmentEffectsRecursively(InvalidUpdates, Pov);
            }
          }
        }

        /* Return the version for the given key, if any. */
        template <typename TKey>
        TVersion<TKey> *TryGetVersion(
            std::unordered_map<TKey, TVersion<TKey> *> &version_by_key,
            const TKey &key) const {
          assert(this);
          /* Do we already have the result cached? */
          auto result = version_by_key.insert(std::pair<TKey, TVersion<TKey> *>(key, 0));
          if (result.second) {
            /* We missed the cache, so search upward to find the key tour closest to our private pov,
               then get the newest version in that tour.  We might come up empty here, meaning that they key
               has not occurred before at all. */
            TTour<TKey> *tour = TTour<TKey>::TryGetTour(Pov, key, SharedLockVec);
            TVersion<TKey> *version = tour ? tour->NewestVersion : 0;
            /* If we got a version, check to see that the version's update has not been invalidated.  If it has been
               invalidated, step backward in time until we find a valid update or reach the start of they key's
               timeline. */
            while (version && InvalidUpdates.find(version->GetCluster()->GetUpdate()) != InvalidUpdates.end()) {
              version = version->TryGetAnyOlderVersion(SharedLockVec);
            }
            /* Cache the result of the search. */
            result.first->second = version;
          }
          return result.first->second;
        }

        /* The leaf point-of-view stack which we are reading. */
        TPov *Pov;

        /* The updates which have been invalidated by backstepping. */
        std::unordered_set<const TUpdate *> InvalidUpdates;

        /* A cache of the actions for nodes we've already looked up. */
        mutable std::unordered_map<TKV, TVersion<TKV> *> VersionByKV;

        /* A list of shared locks we aquired on povs while reading in the order we aquired them. */
        mutable std::vector<TSync::TSharedLock *> SharedLockVec;

      };  // TContext

    }  // FluxCapacitor

  }  // Spa

}  // Stig
