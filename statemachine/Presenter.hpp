#pragma once
#include <statemachine/Process.hpp>
#include <statemachine/CommandFactory.hpp>

#include <Process/Focus/FocusDispatcher.hpp>
#include <Process/LayerPresenter.hpp>
#include <Process/ProcessContext.hpp>
#include <Process/ZoomHelper.hpp>

#include <Process/ProcessContext.hpp>
#include <Process/Tools/ToolPalette.hpp>
#include <Scenario/Palette/Tools/States/MoveAndMergeState.hpp>
#include <Scenario/Palette/Tools/States/ScenarioMoveStatesWrapper.hpp>
#include <Scenario/Palette/Tools/SmartTool.hpp>
#include <Scenario/Palette/Tool.hpp>
#include <Scenario/Palette/ScenarioPoint.hpp>
#include <Scenario/Application/ScenarioEditionSettings.hpp>

#include <Scenario/Document/Event/EventPresenter.hpp>
#include <Scenario/Document/State/StatePresenter.hpp>
#include <Scenario/Document/Interval/IntervalPresenter.hpp>
#include <Scenario/Document/Interval/Temporal/TemporalIntervalPresenter.hpp>
#include <Scenario/Document/TimeSync/TimeSyncPresenter.hpp>

#include <score/command/Dispatchers/OngoingCommandDispatcher.hpp>
#include <score/graphics/GraphicsItem.hpp>
#include <score/model/IdentifiedObjectMap.hpp>
#include <score/model/Identifier.hpp>
#include <score/tools/std/Optional.hpp>
#include <score/statemachine/GraphicsSceneToolPalette.hpp>

#include <statemachine/MoveEvent.hpp>
#include <verdigris>

namespace statemachine
{
class Model;
class View;
class Presenter;
class MoveIntervalInStateMachine_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static auto
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /*
    /// Interval
    /// //TODO remove useless arguments to ctor
    auto moveInterval = new MoveIntervalState<ToolPalette_T>{
        palette,
        palette.model(),
        palette.context().context.commandStack,
        palette.context().context.objectLocker,
        &parent};

    score::make_transition<ClickOnInterval_Transition<Scenario_T>>(
        waitState, moveInterval, *moveInterval);
    moveInterval->addTransition(moveInterval, finishedState(), waitState);
    return moveInterval;
    */

    return nullptr;
  }
};
class MoveEventInStateMachine_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static auto
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// Event
    auto moveEvent = new Scenario::MoveEventState<
        statemachine::MoveEvent,
        Scenario_T,
        ToolPalette_T>{palette,
                       palette.model(),
                       palette.context().context.commandStack,
                       palette.context().context.objectLocker,
                       &parent};

    score::make_transition<Scenario::ClickOnState_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);

    score::make_transition<Scenario::ClickOnEvent_Transition<Scenario_T>>(
        waitState, moveEvent, *moveEvent);
    moveEvent->addTransition(moveEvent, finishedState(), waitState);

    return moveEvent;
  }
};

class MoveTimeSyncInStateMachine_StateWrapper
{
public:
  template <typename Scenario_T, typename ToolPalette_T>
  static auto
  make(const ToolPalette_T& palette, QState* waitState, QState& parent)
  {
    /// TimeSync
    auto moveTimeSync = new Scenario::MoveTimeSyncState<
        statemachine::MoveEvent,
        Scenario_T,
        ToolPalette_T>{palette,
                       palette.model(),
                       palette.context().context.commandStack,
                       palette.context().context.objectLocker,
                       &parent};

    score::make_transition<Scenario::ClickOnTimeSync_Transition<Scenario_T>>(
        waitState, moveTimeSync, *moveTimeSync);

    moveTimeSync->addTransition(moveTimeSync, finishedState(), waitState);

    return moveTimeSync;
  }
};

class ToolPalette final : public GraphicsSceneToolPalette
{
public:
  ToolPalette(Process::LayerContext&, Presenter& presenter);

  const Presenter& presenter() const { return m_presenter; }
  Scenario::EditionSettings& editionSettings() const;

  const Process::LayerContext& context() const { return m_context; }
  Process::MagnetismAdjuster& magnetic() const { return m_magnetic; }

  const Model& model() const { return m_model; }

  void on_pressed(QPointF);
  void on_moved(QPointF);
  void on_released(QPointF);
  void on_cancel();

  void activate(Scenario::Tool);
  void desactivate(Scenario::Tool);

private:
  Scenario::Point ScenePointToScenarioPoint(QPointF point);

  Presenter& m_presenter;
  const Model& m_model;
  Process::LayerContext& m_context;
  Process::MagnetismAdjuster& m_magnetic;

  //CreationTool<Model, ToolPalette> m_createTool;
  Scenario::SmartTool<
      Model,
      ToolPalette,
      View,
      MoveIntervalInStateMachine_StateWrapper,
      Scenario::MoveLeftBraceInScenario_StateWrapper,
      Scenario::MoveRightBraceInScenario_StateWrapper,
      MoveEventInStateMachine_StateWrapper,
      MoveTimeSyncInStateMachine_StateWrapper>
      m_selectTool;

  //PlayToolState m_playTool;

  ToolPaletteInputDispatcher<
      Scenario::Tool,
      ToolPalette,
      Process::LayerContext,
      Process::LayerView>
  m_inputDisp;
};
class ViewInterface
{
public:
  ViewInterface(const Presenter& presenter)
    : m_presenter{presenter}
  {
  }

  void on_eventMoved(const Scenario::EventPresenter& event);
  void on_intervalMoved(const Scenario::TemporalIntervalPresenter& interval);
  void on_timeSyncMoved(const Scenario::TimeSyncPresenter& timesync);
  void on_stateMoved(const Scenario::StatePresenter& state);

  void on_graphicalScaleChanged(double scale);

private:
  const Presenter& m_presenter;
};
class Presenter final
    : public Process::LayerPresenter,
      public Nano::Observer
{
  W_OBJECT(Presenter)

  friend class ToolPalette;
  friend class ViewInterface;

public:
  Presenter(
      Scenario::EditionSettings&,
      const Model& model,
      Process::LayerView* view,
      const Process::Context& context,
      QObject* parent);
  ~Presenter();

  const Model& model() const noexcept;

  /**
   * @brief toScenarioPoint
   *
   * Maps a point in item coordinates
   * to a point in scenario model coordinates (time; y percentage)
   */
  Scenario::Point toScenarioPoint(QPointF pt) const noexcept;
  QPointF fromScenarioPoint(const Scenario::Point& pt) const noexcept;

  void setWidth(qreal width, qreal defaultWidth) override;
  void setHeight(qreal height) override;
  void putToFront() override;
  void putBehind() override;

  void parentGeometryChanged() override;

  void on_zoomRatioChanged(ZoomRatio val) override;

  Scenario::EventPresenter& event(const Id<Scenario::EventModel>& id) const;
  Scenario::TimeSyncPresenter& timeSync(const Id<Scenario::TimeSyncModel>& id) const;
  Scenario::IntervalPresenter& interval(const Id<Scenario::IntervalModel>& id) const;
  Scenario::StatePresenter& state(const Id<Scenario::StateModel>& id) const;
  const auto& getEvents() const { return m_events; }
  const auto& getTimeSyncs() const { return m_timeSyncs; }
  const auto& getIntervals() const { return m_intervals; }
  const auto& getStates() const { return m_states; }

  View& view() const { return *m_view; }
  const ZoomRatio& zoomRatio() const { return m_zoomRatio; }

  ToolPalette& stateMachine() { return m_sm; }
  Scenario::EditionSettings& editionSettings() const
  {
    return m_editionSettings;
  }

  void fillContextMenu(
      QMenu&,
      QPoint pos,
      QPointF scenepos,
      const Process::LayerContextMenuManager&) override;

  bool event(QEvent* e) override { return QObject::event(e); }

  void drawDragLine(const Scenario::StateModel&, Scenario::Point) const;
  void stopDrawDragLine() const;

public:
  void linesExtremityScaled(int arg_1, int arg_2) W_SIGNAL(
      linesExtremityScaled,
      arg_1,
      arg_2)

  void keyPressed(int arg_1)
      W_SIGNAL(keyPressed, arg_1)
  void keyReleased(int arg_1)
      W_SIGNAL(keyReleased, arg_1)

public:
  // Model -> view
  void on_stateCreated(const Scenario::StateModel&);
  void on_stateRemoved(const Scenario::StateModel&);

  void on_eventCreated(const Scenario::EventModel&);
  void on_eventRemoved(const Scenario::EventModel&);

  void on_timeSyncCreated(const Scenario::TimeSyncModel&);
  void on_timeSyncRemoved(const Scenario::TimeSyncModel&);

  void on_intervalCreated(const Scenario::IntervalModel&);
  void on_intervalRemoved(const Scenario::IntervalModel&);

  void on_askUpdate();

  void on_intervalExecutionTimer();

private:

  void selectLeft();
  void selectRight();
  void selectUp();
  void selectDown();

  // void doubleClick(QPointF);
  void on_focusChanged() override;

  template <typename Map, typename Id>
  void removeElement(Map& map, const Id& id);

  void updateAllElements();

  ZoomRatio m_zoomRatio{1};
  double m_graphicalScale{1.};

  // The order of deletion matters!
  // m_view has to be deleted after the other elements.
  graphics_item_ptr<View> m_view{};

  IdContainer<Scenario::StatePresenter, Scenario::StateModel> m_states;
  IdContainer<Scenario::EventPresenter, Scenario::EventModel> m_events;
  IdContainer<Scenario::TimeSyncPresenter, Scenario::TimeSyncModel> m_timeSyncs;
  IdContainer<Scenario::TemporalIntervalPresenter, Scenario::IntervalModel> m_intervals;

  ViewInterface m_viewInterface;

  Scenario::EditionSettings& m_editionSettings;

  OngoingCommandDispatcher m_ongoingDispatcher;

  score::SelectionDispatcher m_selectionDispatcher;
  ToolPalette m_sm;

  QMetaObject::Connection m_con;
};

}
