#include <score/command/Dispatchers/CommandDispatcher.hpp>

#include <statemachine/Presenter.hpp>
#include <statemachine/Process.hpp>
#include <statemachine/View.hpp>

#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>

#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <Scenario/Application/Menus/ScenarioCopy.hpp>
#include <score/selection/SelectionStack.hpp>


#include <QTimer>
W_OBJECT_IMPL(statemachine::Presenter)
namespace statemachine
{



void ViewInterface::on_eventMoved(const Scenario::EventPresenter& ev)
{
  auto h = m_presenter.m_view->boundingRect().height();

  ev.view()->setExtent(ev.extent() * h);

  ev.view()->setPos({ev.model().date().toPixels(m_presenter.m_zoomRatio),
                     ev.extent().top() * h});

  // We also have to move all the relevant states
  for (const auto& state : ev.model().states())
  {
    auto state_it = m_presenter.m_states.find(state);
    if (state_it != m_presenter.m_states.end())
    {
      on_stateMoved(*state_it);
    }
  }
  m_presenter.m_view->update();
}

void ViewInterface::on_intervalMoved(
    const Scenario::TemporalIntervalPresenter& pres)
{
  auto rect = m_presenter.m_view->boundingRect();
  auto msPerPixel = m_presenter.m_zoomRatio;

  const auto& cstr_model = pres.model();
  auto& cstr_view = view(pres);

  double startPos = cstr_model.date().toPixels(msPerPixel);
  // double delta = cstr_view.x() - startPos;
  bool dateChanged = true; // Disabled because it does a whacky movement when
                           // there are processes. (delta * delta > 1); //
                           // Magnetism

  if (dateChanged)
  {
    cstr_view.setPos(startPos, rect.height() * cstr_model.heightPercentage());
  }
  else
  {
    cstr_view.setY(qreal(rect.height() * cstr_model.heightPercentage()));
  }

  cstr_view.setDefaultWidth(
      cstr_model.duration.defaultDuration().toPixels(msPerPixel));
  cstr_view.setMinWidth(
      cstr_model.duration.minDuration().toPixels(msPerPixel));
  cstr_view.setMaxWidth(
      cstr_model.duration.isMaxInfinite(),
      cstr_model.duration.isMaxInfinite()
          ? -1
          : cstr_model.duration.maxDuration().toPixels(msPerPixel));

  m_presenter.m_view->update();
}

void ViewInterface::on_timeSyncMoved(const Scenario::TimeSyncPresenter& timesync)
{
  auto h = m_presenter.m_view->boundingRect().height();
  timesync.view()->setExtent(timesync.extent() * h);

  timesync.view()->setPos(
      {timesync.model().date().toPixels(m_presenter.m_zoomRatio),
       timesync.extent().top() * h});

  m_presenter.m_view->update();
}

void ViewInterface::on_stateMoved(const Scenario::StatePresenter& state)
{
  auto rect = m_presenter.m_view->boundingRect();
  const auto& ev = m_presenter.model().event(state.model().eventId());

  state.view()->setPos({ev.date().toPixels(m_presenter.m_zoomRatio),
                        rect.height() * state.model().heightPercentage()});

  m_presenter.m_view->update();
}













ToolPalette::ToolPalette(
    Process::LayerContext& lay,
    Presenter& presenter)
    : GraphicsSceneToolPalette{*presenter.view().scene()}
    , m_presenter{presenter}
    , m_model{m_presenter.model()}
    , m_context{lay}
    , m_magnetic{(Process::MagnetismAdjuster&)lay.context.app.interfaces<Process::MagnetismAdjuster>()}
    // , m_createTool{*this}
    , m_selectTool{*this}
    // , m_playTool{*this}
    , m_inputDisp{presenter.view(), *this, lay}
{
}

Scenario::EditionSettings& ToolPalette::editionSettings() const
{
  return m_presenter.editionSettings();
}

void ToolPalette::on_pressed(QPointF point)
{
  scenePoint = point;
  auto scenarioPoint
      = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
  switch (editionSettings().tool())
  {
    case Scenario::Tool::Create:
      // m_createTool.on_pressed(point, scenarioPoint);
      break;
    case Scenario::Tool::Playing:
    case Scenario::Tool::Select:
      m_selectTool.on_pressed(point, scenarioPoint);
      break;
    case Scenario::Tool::Play:
      // m_playTool.on_pressed(point, scenarioPoint);
      break;
    default:
      break;
  }
}

void ToolPalette::on_moved(QPointF point)
{
  scenePoint = point;
  auto scenarioPoint
      = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
  switch (editionSettings().tool())
  {
    case Scenario::Tool::Create:
      // m_createTool.on_moved(point, scenarioPoint);
      break;
    case Scenario::Tool::Select:
      m_selectTool.on_moved(point, scenarioPoint);
      break;
    default:
      break;
  }
}

void ToolPalette::on_released(QPointF point)
{
  scenePoint = point;
  auto& es = m_presenter.editionSettings();
  auto scenarioPoint
      = ScenePointToScenarioPoint(m_presenter.m_view->mapFromScene(point));
  switch (es.tool())
  {
    case Scenario::Tool::Create:
      // m_createTool.on_released(point, scenarioPoint);
      es.setTool(Scenario::Tool::Select);
      break;
    case Scenario::Tool::Playing:
      m_selectTool.on_released(point, scenarioPoint);
      es.setTool(Scenario::Tool::Select);
      break;
    case Scenario::Tool::Select:
      m_selectTool.on_released(point, scenarioPoint);
      break;
    default:
      break;
  }
}

void ToolPalette::on_cancel()
{
  // m_createTool.on_cancel();
  m_selectTool.on_cancel();
  m_presenter.editionSettings().setTool(Scenario::Tool::Select);
}

void ToolPalette::activate(Scenario::Tool t) {}

void ToolPalette::desactivate(Scenario::Tool t) {}

Scenario::Point ToolPalette::ScenePointToScenarioPoint(QPointF point)
{
  return Scenario::ConvertToScenarioPoint(
      point,
      m_presenter.zoomRatio(),
      m_presenter.view().boundingRect().height());
}












void updateTimeSyncExtent(Scenario::TimeSyncPresenter& tn);

// Will call updateTimeSyncExtent
void updateEventExtent(
    Presenter& pres,
    Scenario::EventPresenter& ev,
    double view_height);

// Will call updateEventExtent
void updateIntervalVerticalPos(
    Presenter& pres,
    Scenario::IntervalModel& itv,
    double y,
    double view_height);

Presenter::Presenter(
    Scenario::EditionSettings& e,
    const Model& scenario,
    Process::LayerView* view,
    const Process::Context& context,
    QObject* parent)
    : LayerPresenter{scenario, view, context, parent}
    , m_view{static_cast<View*>(view)}
    , m_viewInterface{*this}
    , m_editionSettings{e}
    , m_ongoingDispatcher{context.commandStack}
    , m_selectionDispatcher{context.selectionStack}
    , m_sm{m_context, *this}
{
  // m_view->init(this);
  /////// Setup of existing data
  // For each interval & event, display' em
  for (const auto& tn_model : scenario.timeSyncs)
  {
    on_timeSyncCreated(tn_model);
  }

  for (const auto& event_model : scenario.events)
  {
    on_eventCreated(event_model);
  }

  for (const auto& state_model : scenario.states)
  {
    on_stateCreated(state_model);
  }

  for (const auto& interval : scenario.intervals)
  {
    on_intervalCreated(interval);
  }


  /////// Connections
  scenario.intervals.added.connect<&Presenter::on_intervalCreated>(
      this);
  scenario.intervals.removed.connect<&Presenter::on_intervalRemoved>(
      this);

  scenario.states.added.connect<&Presenter::on_stateCreated>(this);
  scenario.states.removed.connect<&Presenter::on_stateRemoved>(this);

  scenario.events.added.connect<&Presenter::on_eventCreated>(this);
  scenario.events.removed.connect<&Presenter::on_eventRemoved>(this);

  scenario.timeSyncs.added.connect<&Presenter::on_timeSyncCreated>(
      this);
  scenario.timeSyncs.removed.connect<&Presenter::on_timeSyncRemoved>(
      this);

  connect(m_view, &View::keyPressed,
          this, [this] (int k) {
      keyPressed(k);
      switch(k)
      {
      case Qt::Key_Left:
          return selectLeft();
      case Qt::Key_Right:
          return selectRight();
      case Qt::Key_Up:
          return selectUp();
      case Qt::Key_Down:
          return selectDown();
      default:
          break;
      }
  });
  connect(
      m_view,
      &View::keyReleased,
      this,
      &Presenter::keyReleased);

  /*
  connect(
      m_view,
      &View::doubleClicked,
      this,
      &Presenter::doubleClick);
  */
  connect(
      m_view,
      &View::askContextMenu,
      this,
      &Presenter::contextMenuRequested);
  /*
  connect(
      m_view,
      &View::dragEnter,
      this,
      [=](const QPointF& pos, const QMimeData& mime) {
        try
        {
          m_context.context.app.interfaces<Scenario::DropHandlerList>()
              .dragEnter(*this, pos, mime);
        }
        catch (std::exception& e)
        {
          qDebug() << "Error during dragEnter: " << e.what();
        }
      });
  connect(
      m_view,
      &View::dragMove,
      this,
      [=](const QPointF& pos, const QMimeData& mime) {
        try
        {
          m_context.context.app.interfaces<Scenario::DropHandlerList>()
              .dragMove(*this, pos, mime);
        }
        catch (std::exception& e)
        {
          qDebug() << "Error during dragMove: " << e.what();
        }
      });
  connect(
      m_view,
      &View::dragLeave,
      this,
      [=](const QPointF& pos, const QMimeData& mime) {
        try
        {
          stopDrawDragLine();
          m_context.context.app.interfaces<Scenario::DropHandlerList>()
              .dragLeave(*this, pos, mime);
        }
        catch (std::exception& e)
        {
          qDebug() << "Error during dragLeave: " << e.what();
        }
      });
  connect(
      m_view,
      &View::dropReceived,
      this,
      [=](const QPointF& pos, const QMimeData& mime) {
        try
        {
          stopDrawDragLine();
          m_context.context.app.interfaces<Scenario::DropHandlerList>().drop(
              *this, pos, mime);
        }
        catch (std::exception& e)
        {
          qDebug() << "Error during drop: " << e.what();
        }
      });
  */

  m_con = con(
      context.execTimer,
      &QTimer::timeout,
      this,
      &Presenter::on_intervalExecutionTimer);

  auto& es = context.app.guiApplicationPlugin<Scenario::ScenarioApplicationPlugin>()
                 .editionSettings();
  con(es, &Scenario::EditionSettings::toolChanged, this, [=](Scenario::Tool t) {
    switch (t)
    {
      case Scenario::Tool::Select:
        m_view->unsetCursor();
        break;
      case Scenario::Tool::Create:
        break;
      case Scenario::Tool::Play:
        m_view->setCursor(QCursor(Qt::PointingHandCursor));
        break;
      default:
        m_view->unsetCursor();
        break;
    }
  });
}

Presenter::~Presenter()
{
  disconnect(m_con);
  m_intervals.remove_all();
  m_states.remove_all();
  m_events.remove_all();
  m_timeSyncs.remove_all();
}

const Model& Presenter::model() const noexcept
{
  return static_cast<const Model&>(m_process);
}

Scenario::Point Presenter::toScenarioPoint(QPointF pt) const noexcept
{
  return Scenario::ConvertToScenarioPoint(pt, zoomRatio(), view().height());
}

QPointF Presenter::fromScenarioPoint(const Scenario::Point& pt) const noexcept
{
  return ConvertFromScenarioPoint(pt, zoomRatio(), view().height());
}

void Presenter::setWidth(qreal width, qreal defaultWidth)
{
  m_view->setWidth(width);
}

void Presenter::setHeight(qreal height)
{
  m_view->setHeight(height);
  if(height > 10)
  {
    for(auto& ev : m_events)
    {
      updateEventExtent(*this, ev, height);
    }
  }
}

void Presenter::putToFront()
{
  //m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  m_view->setOpacity(1);
}

void Presenter::putBehind()
{
  //m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
  m_view->setOpacity(0.1);
}

void Presenter::parentGeometryChanged()
{
  updateAllElements();
  m_view->update();
}

void Presenter::on_zoomRatioChanged(ZoomRatio val)
{
  m_zoomRatio = val;

  for (auto& interval : m_intervals)
  {
    interval.on_zoomRatioChanged(m_zoomRatio);
  }
}

Scenario::TimeSyncPresenter&
Presenter::timeSync(const Id<Scenario::TimeSyncModel>& id) const
{
  return m_timeSyncs.at(id);
}

Scenario::IntervalPresenter&
Presenter::interval(const Id<Scenario::IntervalModel>& id) const
{
  return m_intervals.at(id);
}

Scenario::StatePresenter& Presenter::state(const Id<Scenario::StateModel>& id) const
{
  return m_states.at(id);
}

void Presenter::fillContextMenu(QMenu&, QPoint pos, QPointF scenepos, const Process::LayerContextMenuManager&)
{

}

Scenario::EventPresenter& Presenter::event(const Id<Scenario::EventModel>& id) const
{
  return m_events.at(id);
}

template <typename Map, typename Id>
void Presenter::removeElement(Map& map, const Id& id)
{
  map.erase(id);
  m_view->update();
}

void Presenter::on_stateRemoved(const Scenario::StateModel& state)
{
  Scenario::EventPresenter& ev = m_events.at(state.eventId());
  ev.removeState(&m_states.at(state.id()));

  updateEventExtent(*this, ev, m_view->height());

#if defined(SCORE_DEBUG)
  for(auto& ev : m_events)
  {
    for(auto st : ev.states())
    {
      SCORE_ASSERT(st->id() != state.id());
    }
  }
#endif
  removeElement(m_states, state.id());
}

void Presenter::on_eventRemoved(const Scenario::EventModel& event)
{
  Scenario::TimeSyncPresenter& ts = m_timeSyncs.at(event.timeSync());
  ts.removeEvent(&m_events.at(event.id()));
  updateTimeSyncExtent(ts);

  removeElement(m_events, event.id());
}

void Presenter::on_timeSyncRemoved(const Scenario::TimeSyncModel& timeSync)
{
  removeElement(m_timeSyncs, timeSync.id());
}

void Presenter::on_intervalRemoved(const Scenario::IntervalModel& cvm)
{
  removeElement(m_intervals, cvm.id());
}


/////////////////////////////////////////////////////////////////////
// USER INTERACTIONS
void Presenter::on_askUpdate()
{
  m_view->update();
}

void Presenter::on_intervalExecutionTimer()
{
  // TODO optimize me by storing a list of the currently running intervals
  // TOOD loop

  for (Scenario::TemporalIntervalPresenter& cst : m_intervals)
  {
    auto& v = *cst.view();
    auto& dur = cst.model().duration;

    auto pp = cst.model().duration.playPercentage();

    if (double w = cst.on_playPercentageChanged(pp))
    {
      const auto r = v.boundingRect();

      if (r.width() > 7.)
      {
        QRectF toUpdate = {r.x() + v.playWidth() - w, r.y(), 2. * w, 6.};
        if(!dur.isRigid())
        {
          double new_w = dur.isMaxInfinite()
              ? v.defaultWidth() - v.playWidth() + 2. * w
              : v.maxWidth() - v.playWidth() + 2. * w;
          toUpdate.setWidth(new_w);
        }
        v.update(toUpdate);
      }
      else if (pp == 0.)
      {
        v.update();
      }
    }
    else if(!dur.isRigid())
    {
      // We need to update in that case because
      // of the pulsing color of the interval
      const auto r = v.boundingRect();
      double new_w = dur.isMaxInfinite()
          ? v.defaultWidth() - v.minWidth() + 4.
          : v.maxWidth() - v.minWidth() + 4.;
      QRectF toUpdate = {r.x() + v.minWidth() - 2., r.y(), new_w, 6.};
      v.update(toUpdate);
    }
  }
}

void Presenter::selectLeft()
{
    Scenario::CategorisedScenario selection{this->model()};

    const auto n_itvs = selection.selectedIntervals.size();
    const auto n_ev = selection.selectedEvents.size();
    const auto n_states = selection.selectedStates.size();
    const auto n_syncs = selection.selectedTimeSyncs.size();
    switch(n_itvs + n_ev + n_states + n_syncs)
    {
    case 1:
    {
        if(n_itvs == 1)
        {
            auto& itv = *selection.selectedIntervals.front();
            auto& left_state = Scenario::startState(itv, model());
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&left_state});
        }
        else if(n_states == 1)
        {
            const Scenario::StateModel& st = *selection.selectedStates.front();
            if(st.previousInterval())
            {
                auto& left_itv = Scenario::previousInterval(st, model());
                score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&left_itv});
            }
        }
        break;
    }
    default:
        break;
    }
}


void Presenter::selectRight()
{
    Scenario::CategorisedScenario selection{this->model()};

    const auto n_itvs = selection.selectedIntervals.size();
    const auto n_ev = selection.selectedEvents.size();
    const auto n_states = selection.selectedStates.size();
    const auto n_syncs = selection.selectedTimeSyncs.size();
    switch(n_itvs + n_ev + n_states + n_syncs)
    {
    case 1:
    {
        if(n_itvs == 1)
        {
            auto& itv = *selection.selectedIntervals.front();
            auto& left_state = Scenario::endState(itv, model());
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&left_state});
        }
        else if(n_states == 1)
        {
            const Scenario::StateModel& st = *selection.selectedStates.front();
            if(st.nextInterval())
            {
                auto& left_itv = Scenario::nextInterval(st, model());
                score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&left_itv});
            }
        }
        break;
    }
    default:
        break;
    }
}

// TODO MOVEME
template<typename Scenario_T>
ossia::small_vector<Scenario::StateModel*, 8> getStates(const Scenario::TimeSyncModel& ts, const Scenario_T& scenario)
{
    ossia::small_vector<Scenario::StateModel*, 8> states;
    states.reserve(ts.events().size() * 2);
    for(auto& ev : ts.events()) {
        auto& e = scenario.event(ev);
        for(auto& state : e.states()) {
            states.push_back(&scenario.state(state));
        }
    }
    return states;
}

void Presenter::selectUp()
{
    Scenario::CategorisedScenario selection{this->model()};

    const auto n_itvs = selection.selectedIntervals.size();
    const auto n_ev = selection.selectedEvents.size();
    const auto n_states = selection.selectedStates.size();
    const auto n_syncs = selection.selectedTimeSyncs.size();

    if(n_states == 1)
    {
        if(n_itvs || n_ev || n_syncs)
            return;

        const auto& sel_state = *selection.selectedStates.front();
        const auto& parent_ts = Scenario::parentTimeSync(sel_state, model());
        const auto states = statemachine::getStates(parent_ts, model());
        double min = -1.;
        const auto* cur_state = &sel_state;
        for(Scenario::StateModel* state : states)
        {
            const auto h = state->heightPercentage();
            if(h < sel_state.heightPercentage() && h > min) {
                cur_state = state;
                min = h;
            }
        }

        if(cur_state != &sel_state) {
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({cur_state});
        }
        else {
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&parent_ts});
        }
    }
    else
    {
        auto sel = m_context.context.selectionStack.currentSelection();
        if(sel.empty())
            return;
        for(auto& ptr : sel) {
            if(!ptr)
                continue;
            auto proc = qobject_cast<const Process::ProcessModel*>(ptr);
            if(!proc)
                continue;

            auto parent = ptr->parent();
            if(!parent)
                continue;
            if(auto gp = parent->parent(); gp != &model())
                continue;

            auto itv = qobject_cast<Scenario::IntervalModel*>(parent);
            if(!itv)
                continue;

            const auto& rack = itv->smallView();
            std::size_t i = 0;
            for(const Scenario::Slot& slot : rack)
            {
                if(ossia::contains(slot.processes, proc->id()))
                {
                    if(i > 0)  // at minimum zero since we're in a slot
                    {
                        if(const auto& prev_proc = rack[i-1].frontProcess)
                        {
                            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&itv->processes.at(*prev_proc)});
                        }
                    }
                    else if(i == 0)
                    {
                        score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({itv});
                    }
                }
                i++;
            }
        }
    }
}


void Presenter::selectDown()
{
    Scenario::CategorisedScenario selection{this->model()};

    const auto n_itvs = selection.selectedIntervals.size();
    const auto n_ev = selection.selectedEvents.size();
    const auto n_states = selection.selectedStates.size();
    const auto n_syncs = selection.selectedTimeSyncs.size();

    if(n_syncs == 1) {
        // Select the topmost state
        if(n_itvs || n_ev || n_states)
            return;

        const auto& sel_sync = *selection.selectedTimeSyncs.front();
        const auto states = statemachine::getStates(sel_sync, model());
        if(states.empty())
            return;

        double max = std::numeric_limits<double>::max();
        const Scenario::StateModel* cur_state = states.front();
        for(Scenario::StateModel* state : states)
        {
            const auto h = state->heightPercentage();
            if(h < max) {
                cur_state = state;
                max = h;
            }
        }

        score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({cur_state});
    }
    else if(n_itvs == 1)
    {
        // Select the process in the first slot, if any
        const Scenario::IntervalModel& itv = *selection.selectedIntervals.front();
        if(itv.processes.empty())
            return;
        if(!itv.smallViewVisible())
            return;
        auto& slot = itv.getSmallViewSlot(0);
        auto& front = slot.frontProcess;
        if(front)
        {
            auto& proc = itv.processes.at(*front);
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&proc});
        }
    }
    else if(n_states == 1) {
        if(n_itvs || n_ev || n_syncs)
            return;

        const auto& sel_state = *selection.selectedStates.front();
        const auto& parent_ts = Scenario::parentTimeSync(sel_state, model());
        const auto states = statemachine::getStates(parent_ts, model());
        double max = std::numeric_limits<double>::max();
        const auto* cur_state = &sel_state;
        for(Scenario::StateModel* state : states)
        {
            const auto h = state->heightPercentage();
            if(h > sel_state.heightPercentage() && h < max) {
                cur_state = state;
                max = h;
            }
        }

        if(cur_state != &sel_state)
            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({cur_state});
    }
    else
    {
        auto sel = m_context.context.selectionStack.currentSelection();
        if(sel.empty())
            return;
        for(auto& ptr : sel) {
            if(!ptr)
                continue;
            auto proc = qobject_cast<const Process::ProcessModel*>(ptr);
            if(!proc)
                continue;

            auto parent = ptr->parent();
            if(!parent)
                continue;
            if(auto gp = parent->parent(); gp != &model())
                continue;

            auto itv = qobject_cast<Scenario::IntervalModel*>(parent);
            if(!itv)
                continue;

            const auto& rack = itv->smallView();
            std::size_t i = 0;
            for(const Scenario::Slot& slot : rack)
            {
                if(ossia::contains(slot.processes, proc->id()))
                {
                    if(i < rack.size() - 1)  // at minimum zero since we're in a slot
                    {
                        if(const auto& next_proc = rack[i+1].frontProcess)
                        {
                            score::SelectionDispatcher{m_context.context.selectionStack}.setAndCommit({&itv->processes.at(*next_proc)});
                        }
                    }
                }
                i++;
            }
        }
    }
}


void Presenter::on_focusChanged()
{
  if (focused())
  {
    m_view->setFocus();
  }

  editionSettings().setTool(Scenario::Tool::Select);
  editionSettings().setExpandMode(ExpandMode::Scale);
}

/////////////////////////////////////////////////////////////////////
// ELEMENTS CREATED
void Presenter::on_eventCreated(const Scenario::EventModel& event_model)
{
  auto ev_pres = new Scenario::EventPresenter{event_model, m_view, this};
  m_events.insert(ev_pres);

  Scenario::TimeSyncPresenter& ts = m_timeSyncs.at(event_model.timeSync());
  ts.addEvent(ev_pres);
  ev_pres->view()->setWidthScale(m_graphicalScale);
  m_viewInterface.on_eventMoved(*ev_pres);

  con(*ev_pres,
      &Scenario::EventPresenter::recomputeExtent,
      this,
      [this, ev_pres] {
    updateEventExtent(*this, *ev_pres, m_view->height());
  });
  con(*ev_pres,
      &Scenario::EventPresenter::extentChanged,
      this,
      [=](const Scenario::VerticalExtent&) { m_viewInterface.on_eventMoved(*ev_pres); });
  con(event_model, &Scenario::EventModel::dateChanged, this, [=](const TimeVal&) {
    m_viewInterface.on_eventMoved(*ev_pres);
  });

  // For the state machine
  connect(
      ev_pres, &Scenario::EventPresenter::pressed, m_view, &View::pressedAsked);
  connect(ev_pres, &Scenario::EventPresenter::moved, m_view, &View::movedAsked);
  connect(ev_pres, &Scenario::EventPresenter::released, m_view, &View::released);
}

void Presenter::on_timeSyncCreated(const Scenario::TimeSyncModel& timeSync_model)
{
  auto tn_pres = new Scenario::TimeSyncPresenter{timeSync_model, m_view, this};
  m_timeSyncs.insert(tn_pres);

  m_viewInterface.on_timeSyncMoved(*tn_pres);

  con(*tn_pres,
      &Scenario::TimeSyncPresenter::recomputeExtent,
      this,
      [tn_pres] {
    updateTimeSyncExtent(*tn_pres);
  });
  con(*tn_pres,
      &Scenario::TimeSyncPresenter::extentChanged,
      this,
      [=](const Scenario::VerticalExtent&) {
        m_viewInterface.on_timeSyncMoved(*tn_pres);
      });
  con(timeSync_model, &Scenario::TimeSyncModel::dateChanged, this, [=](const TimeVal&) {
    m_viewInterface.on_timeSyncMoved(*tn_pres);
  });

  // For the state machine
  connect(
      tn_pres,
      &Scenario::TimeSyncPresenter::pressed,
      m_view,
      &View::pressedAsked);
  connect(
      tn_pres, &Scenario::TimeSyncPresenter::moved, m_view, &View::movedAsked);
  connect(
      tn_pres, &Scenario::TimeSyncPresenter::released, m_view, &View::released);
}

void Presenter::on_stateCreated(const Scenario::StateModel& state)
{
  auto st_pres = new Scenario::StatePresenter{state, m_context.context, m_view, this};
  m_states.insert(st_pres);

  st_pres->view()->setScale(m_graphicalScale);
  m_viewInterface.on_stateMoved(*st_pres);

  Scenario::EventPresenter& ev_pres = m_events.at(state.eventId());
/*
  for(auto& ev : m_events)
  {
    for(auto st : ev.states())
    {
      SCORE_ASSERT(st->id() != state.id());
    }
  }*/
  ev_pres.addState(st_pres);

  con(state, &Scenario::StateModel::heightPercentageChanged, this, [this, st_pres] {
    m_viewInterface.on_stateMoved(*st_pres);
    updateEventExtent(*this, m_events.at(st_pres->model().eventId()), m_view->height());
  });
  con(state, &Scenario::StateModel::eventChanged, this, [this, st_pres] (const auto& oldid, const auto& newid) {
    Scenario::EventPresenter& oldev = m_events.at(oldid);
    Scenario::EventPresenter& newev = m_events.at(newid);
    const auto h = m_view->height();
    oldev.removeState(st_pres);
    newev.addState(st_pres);

    updateEventExtent(*this, oldev, h);
    updateEventExtent(*this, newev, h);
  });
  updateEventExtent(*this, ev_pres, m_view->height());

  // For the state machine
  connect(
      st_pres, &Scenario::StatePresenter::pressed, m_view, &View::pressedAsked);
  connect(st_pres, &Scenario::StatePresenter::moved, m_view, &View::movedAsked);
  connect(st_pres, &Scenario::StatePresenter::released, m_view, &View::released);
}

void Presenter::on_intervalCreated(const Scenario::IntervalModel& interval)
{
  auto cst_pres = new Scenario::TemporalIntervalPresenter{
      interval, m_context.context, true, m_view, this};
  m_intervals.insert(cst_pres);
  cst_pres->on_zoomRatioChanged(m_zoomRatio);

  m_viewInterface.on_intervalMoved(*cst_pres);

  con(interval,
      &Scenario::IntervalModel::requestHeightChange,
      this,
      [this, &interval] (double y) {
    updateIntervalVerticalPos(*this, const_cast<Scenario::IntervalModel&>(interval), y, m_view->height());
  });

  auto& startEvent = Scenario::startEvent(interval, model());
  auto& endEvent = Scenario::endEvent(interval, model());
  auto& startEventPres = m_events.at(startEvent.id());
  auto& endEventPres = m_events.at(endEvent.id());

  con(startEvent, &Scenario::EventModel::statusChanged, cst_pres,
      [cst_pres] { cst_pres->view()->update(); });
  con(endEvent, &Scenario::EventModel::statusChanged, cst_pres,
      [cst_pres] { cst_pres->view()->update(); });

  auto updateHeight = [&] {
    auto h = m_view->height();
    updateEventExtent(*this, startEventPres, h);
    updateEventExtent(*this, endEventPres, h);
  };
  updateEventExtent(*this, startEventPres, m_view->height());
  updateEventExtent(*this, endEventPres, m_view->height());
  con(interval, &Scenario::IntervalModel::rackChanged,
      this, updateHeight);

  con(interval, &Scenario::IntervalModel::smallViewVisibleChanged,
      this, updateHeight);

  con(interval, &Scenario::IntervalModel::slotResized,
      this, updateHeight);

  con(interval, &Scenario::IntervalModel::slotAdded,
      this, updateHeight);
  con(interval, &Scenario::IntervalModel::slotRemoved,
      this, updateHeight);

  connect(
      cst_pres,
      &Scenario::TemporalIntervalPresenter::heightPercentageChanged,
      this,
      [=]() { m_viewInterface.on_intervalMoved(*cst_pres); });
  con(interval, &Scenario::IntervalModel::dateChanged, this, [=](const TimeVal&) {
    m_viewInterface.on_intervalMoved(*cst_pres);
  });
  connect(
      cst_pres,
      &Scenario::TemporalIntervalPresenter::askUpdate,
      this,
      &Presenter::on_askUpdate);

  // For the state machine
  connect(
      cst_pres,
      &Scenario::TemporalIntervalPresenter::pressed,
      m_view,
      &View::pressedAsked);
  connect(
      cst_pres,
      &Scenario::TemporalIntervalPresenter::moved,
      m_view,
      &View::movedAsked);
  connect(
      cst_pres,
      &Scenario::TemporalIntervalPresenter::released,
      m_view,
      &View::released);
}

void Presenter::updateAllElements()
{
  for (auto& interval : m_intervals)
  {
    m_viewInterface.on_intervalMoved(interval);
  }

  for (auto& event : m_events)
  {
    m_viewInterface.on_eventMoved(event);
  }

  for (auto& timesync : m_timeSyncs)
  {
    m_viewInterface.on_timeSyncMoved(timesync);
  }
}

}
















namespace statemachine
{

void updateTimeSyncExtent(Scenario::TimeSyncPresenter& tn)
{
  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::lowest();
  for (const Scenario::EventPresenter* ev : tn.events())
  {
    if (ev->extent().top() < min)
      min = ev->extent().top();
    if (ev->extent().bottom() > max)
      max = ev->extent().bottom();
  }

  if(max - min > 3.)
  {
    min += 1.;
    max -= 1.;
  }

  tn.setExtent({min, max});
}

void updateEventExtent(
    Presenter& pres,
    Scenario::EventPresenter& ev,
    double view_height)
{
  if(view_height <= 2.)
    return;

  auto& s = pres.model();
  double min = std::numeric_limits<double>::max();
  double max = std::numeric_limits<double>::lowest();

  for (Scenario::StatePresenter* stp: ev.states())
  {
    auto& st = stp->model();

    if (st.heightPercentage() < min)
      min = st.heightPercentage();
    if (st.heightPercentage() > max)
      max = st.heightPercentage();

    if (const auto& itv_id = st.previousInterval())
    {
      auto itv = s.intervals.find(*itv_id);
      if(itv == s.intervals.end())
        return;

      const double h = (1. + itv->getHeight()) / view_height;
      if(itv->smallViewVisible() && st.heightPercentage() + h > max)
      {
        max = st.heightPercentage() + h;
      }
    }
    if (const auto& itv_id = st.nextInterval())
    {
      auto itv = s.intervals.find(*itv_id);
      if(itv == s.intervals.end())
        return;
      const double h = (1. + itv->getHeight()) / view_height;
      if(itv->smallViewVisible() && st.heightPercentage() + h > max)
      {
        max = st.heightPercentage() + h;
      }
    }
  }

  ev.setExtent({min, max});
  // TODO we could maybe skip this in case where the event
  // grows ?
  updateTimeSyncExtent(pres.timeSync(ev.model().timeSync()));
}

void updateIntervalVerticalPos(
    Presenter& pres,
    Scenario::IntervalModel& itv,
    double y,
    double view_height)
{
  // TODO why isn't this a command
  if(view_height <= 2.)
    return;

  auto& s = pres.model();
  // First make the list of all the intervals to update
  static ossia::flat_set<Scenario::IntervalModel*> intervalsToUpdate;
  static ossia::flat_set<Scenario::StateModel*> statesToUpdate;

  intervalsToUpdate.insert(&itv);
  Scenario::StateModel* rec_state = &s.state(itv.startState());

  statesToUpdate.insert(rec_state);
  while (auto prev_itv = rec_state->previousInterval())
  {
    Scenario::IntervalModel* rec_cst = &s.intervals.at(*prev_itv);
    intervalsToUpdate.insert(rec_cst);
    statesToUpdate.insert(rec_state);
    rec_state = &s.states.at(rec_cst->startState());
  }
  statesToUpdate.insert(rec_state); // Add the first state

  rec_state = &s.state(itv.endState());
  statesToUpdate.insert(rec_state);
  while (auto next_itv =rec_state->nextInterval())
  {
    Scenario::IntervalModel* rec_cst = &s.intervals.at(*next_itv);
    intervalsToUpdate.insert(rec_cst);
    statesToUpdate.insert(rec_state);
    rec_state = &s.states.at(rec_cst->endState());
  }
  statesToUpdate.insert(rec_state); // Add the last state

  // Set the correct height
  for (auto& interval : intervalsToUpdate)
  {
    interval->setHeightPercentage(y);
    // TODO s.intervalMoved(*interval);
  }

  for (auto& state : statesToUpdate)
  {
    state->setHeightPercentage(y);
    updateEventExtent(pres, pres.event(state->eventId()), view_height);
  }

  intervalsToUpdate.clear();
  statesToUpdate.clear();
}
}
