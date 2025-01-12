(define (domain localization)

	(:requirements :typing :durative-actions :numeric-fluents :negative-preconditions :action-costs :conditional-effects :equality :fluents :disjunctive-preconditions)

	(:types
		robot region
	)

	(:predicates
		(robot_in ?v - robot ?r - region)
		(visited ?r - region)

	)

	(:functions
		(act-cost)
		(triggered ?from ?to - region)
		(dummy)
		(assignment_taked ?v - robot)
		(assignment_region ?r - region)
	)

	(:durative-action goto_region
		:parameters (?v - robot ?from ?to - region)
		:duration (= ?duration 100)
		:condition (and (at start (robot_in ?v ?from)))
		:effect (and (at start (not (robot_in ?v ?from))) (at start (increase (triggered ?from ?to) 1))
			(at end (robot_in ?v ?to)) (at end (assign (triggered ?from ?to) 0)) (at end (visited ?to))
			(at end (increase (act-cost) (dummy))))
	)

	(:action take_assignment
		:parameters (?v - robot ?at - region)
		:precondition (and (robot_in ?v ?at) (> (assignment_region ?at) 0))
		:effect (and (increase (assignment_taked ?v) 1) (decrease (assignment_region ?at) 1))
	)
)